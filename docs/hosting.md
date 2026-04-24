Hosting the download page with GitHub Pages

1. Keep the `web/` folder in the repository root.
2. In GitHub repository settings, enable Pages and select the branch plus the `/web` folder.
3. Publish release builds separately. Do not commit large binaries to source control for long-term use.
4. Update `web/index.html` so the download button points at the latest release ZIP.

Recommended release flow
- Build the Windows artifact from the `PvP_BlockChain/` project.
- Upload the ZIP to GitHub Releases.
- Point the download button at that release asset URL.
