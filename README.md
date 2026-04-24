# Gladiator Arena

Gladiator Arena is a hybrid project: a native SFML fighting game paired with an on-chain backend for character ownership, battle rewards, weapons, currency, battle trophies, and marketplace listings (contracts + Node bridge).

## What is in this repo

- **`PvP_BlockChain/`** — Main C++/SFML game client (Windows HTTP for bridge calls).
- **`blockchain/`** — Hardhat project; contracts live in `contracts/GladiatorArenaSystem.sol`.
- **`node_bridge/`** — Express server that signs transactions with a deployer key for local demos.
- **`web/`** — Static download page for packaged builds.
- **`GladiatorSFML/`** — **Experimental / optional** ImGui + SFML menu shell (not wired to the bridge or the main game loop). Use **`PvP_BlockChain`** as the supported client.

## Current product scope (what is actually wired)

| Feature | Status |
|--------|--------|
| Local two-player combat | Implemented in `PvP_BlockChain`. |
| Wallet addresses on main menu | Used as `player1` / `player2` in battle settlement. |
| Optional **character token IDs** (digits) on main menu | Sent as `winnerCharacterId` to the bridge so `BattleArena` can call `recordVictory` for the winning player’s gladiator when set. |
| Post-match settlement + trophy + AGLD reward | Game → `POST /mintNFT` on the bridge → `BattleArena.settleBattle`. |
| Store → bridge (demo) | **Wallet** submenu: ping `/health`, mint demo AGLD to Player1 via `/currency/mint`. **Weapons** submenu: mint Iron Sword (id 1) or Aegis Shield (id 2) via `/weapons/mint` to Player1’s wallet. **Offline upgrades** submenu: local-only gold (not on-chain). |
| Marketplace list/buy (characters / weapons) | Implemented **in contracts and bridge HTTP API** only; not exposed inside the SFML UI (use curl/Postman or a small dapp against the bridge for demos). |

## Bridge configuration (game client)

The client reads optional JSON at **`Resources/bridge_config.json`** (search path includes the working directory and `PvP_BlockChain/` — see `ResourcePaths.cpp`). If the file is missing, defaults match local bridge defaults.

Copy and edit the example:

- [`PvP_BlockChain/Resources/bridge_config.example.json`](PvP_BlockChain/Resources/bridge_config.example.json) → `Resources/bridge_config.json`

Supported keys:

- `baseUrl` — e.g. `http://localhost:3000`
- `mintPath` — default `/mintNFT`
- `weaponName` — string sent in the battle settlement JSON (metadata / display)

## Architecture

1. The SFML game runs locally and resolves a match.
2. The winner confirms settlement in the client.
3. The client posts JSON to `node_bridge` (URL from `bridge_config.json`).
4. The bridge calls `BattleArena.settleBattle` on-chain.
5. `BattleArena` mints arena currency to the winner, mints a trophy NFT, and optionally calls `GladiatorCharacter.recordVictory` when `winnerCharacterId` is non-zero.

## Local setup

### 1. Blockchain

```powershell
cd blockchain
copy .env.example .env
npm install
npm run compile
npm test
```

Start a local chain in a separate shell if needed, then deploy:

```powershell
npm run deploy:local
```

That writes `blockchain/deployments.generated.json` and `node_bridge/contracts.generated.json` (when the path is writable).

### 2. Bridge

```powershell
cd node_bridge
copy .env.example .env
npm install
npm start
```

### 3. Game client

```powershell
cd PvP_BlockChain
cmake -S . -B build
cmake --build build --config Release
```

Run the built executable with the `Resources/` folder beside it (or from a directory where `findResourcePath` can resolve `Resources/`).

## Platform notes

- **HTTP in the game** is implemented with **WinHTTP** (`PvP_BlockChain/http_client.cpp`). Non-Windows builds compile but bridge calls return an error string unless you add a portable HTTP backend.

## Important note on marketplace endpoints

The bridge includes marketplace endpoints that accept raw private keys. That is only for local private-chain demos. A real production deployment would move those actions to user-signed wallet transactions rather than server-side key handling.

Battle settlement from the game is also **server-signed** via `PRIVATE_KEY` on the bridge — appropriate for local testing only.
