# Prepare repository for GitHub push

This file lists safe steps to remove local secrets, add a .gitignore, and push the project to GitHub from Windows PowerShell.

1) Check for secrets

- Inspect the following files and remove or replace private keys before committing:
  - `node_bridge/ganache_keys.json` (contains private keys)
  - any `*.json` or `*.env` files with keys

2) Replace secret files with samples

- Move the real keys somewhere safe (or encrypt them) and add a sample file:

  # PowerShell
  Move-Item node_bridge\ganache_keys.json node_bridge\ganache_keys.json.local
  Copy-Item node_bridge\ganache_keys.json.sample node_bridge\ganache_keys.json

  Environment variables (bridge private key)

  - `node_bridge/bridge.js` now requires `BRIDGE_PRIVATE_KEY` to be set in the environment instead of hardcoding it.

    You can set it in PowerShell for one session:

    $env:BRIDGE_PRIVATE_KEY = '0xYOURPRIVATEKEY'

    Or create a local file that you don't commit (example `.env.local` — never commit this):

    # node_bridge/.env.local (DO NOT COMMIT)
    BRIDGE_PRIVATE_KEY=0xYOURPRIVATEKEY

    Then load it into your session (PowerShell):

    Get-Content node_bridge\.env.local | ForEach-Object { if ($_ -match "^\s*([^=]+)=(.*)$") { Set-Item -Path Env:\$($matches[1]) -Value $matches[2] } }


3) If you've already committed secrets, remove them from git history

- This rewrites history — only do this if necessary and coordinate with collaborators.

  # PowerShell (run in repo root)
  git rm --cached node_bridge/ganache_keys.json
  git commit -m "Remove ganache keys from repo"
  git push origin --force

4) Add and commit .gitignore

  git add .gitignore node_bridge/ganache_keys.json.sample README_PREPARE_TO_PUSH.md
  git commit -m "chore: add .gitignore and prepare secrets sample"

5) Create GitHub repo and push

- If you don't have a remote yet, create one on GitHub, then:

  git remote add origin https://github.com/<your-user>/<your-repo>.git
  git branch -M main
  git push -u origin main

6) Optional: add a release checklist

- Add a small checklist for future pushes (tests, build) and CI.
