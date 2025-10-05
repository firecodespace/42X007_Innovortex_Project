Place your built ZIP file here and name it `GladiatorArena-latest.zip`.

The landing page (`index.html`) links to `/releases/GladiatorArena-latest.zip` so visitors can download directly.

To automate this via CI, have your packaging step copy the produced ZIP into `web/releases/GladiatorArena-latest.zip` before committing/publishing the site, or upload the ZIP to GitHub Releases and update the link in `web/index.html` to the release asset URL.
