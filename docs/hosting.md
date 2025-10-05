Hosting the download page with GitHub Pages

1) Place the `web/` folder at the repository root (already present in this repo).

2) In repository Settings -> Pages, set the source to `main` branch and folder `/web` (or `/docs` if you move files there).

3) After enabling, your site will be published at `https://<owner>.github.io/<repo>/`.

4) To provide direct downloads use GitHub Releases:
   - Create a release on GitHub (Tags -> Create new release), upload the ZIP produced by the CI or your local packaging script.
   - Copy the release asset download URL and update `web/index.html` -> replace `#RELEASE_ASSET_URL#` with your asset URL.

5) To automate releases using the workflow:
   - Tag a commit e.g. `git tag v0.1.0 && git push origin v0.1.0` to trigger the workflow in `.github/workflows/windows-release.yml`.
   - Download the artifact from the workflow run and attach it to a GitHub Release.

   Warning: Committing large binary build artifacts into the repository (for example into `web/releases`) will increase the repository size over time. The workflow will copy the ZIP into `web/releases` for convenience, but for long-term hosting prefer attaching the ZIP to a GitHub Release or using an external CDN/storage (S3, DigitalOcean Spaces) and point the download link there.

Optional: Use GitHub Releases API to attach artifacts automatically (requires additional workflow steps and a GitHub token).

*** End Patch