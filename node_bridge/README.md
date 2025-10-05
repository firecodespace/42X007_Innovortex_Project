Node bridge README

Setup
- Copy `.env.example` to `.env` and fill RPC_URL, PRIVATE_KEY, CONTRACT_ADDRESS (or leave CONTRACT_ADDRESS blank to deploy via test script), PLAYER1/2 keys from Ganache, and NFT_STORAGE_API_KEY if you want metadata uploaded.

Install deps

  npm install

Start bridge

  npm start

Test scripts
- `npm run test-mint` will POST a sample payload to the running bridge (edit addresses in test_mint.js before running).
- `node test/deploy_and_run.js` will attempt to deploy a compiled `GameNFT.bin` and then POST to the bridge. You must compile the contract and place its bytecode into `node_bridge/GameNFT.bin` first.

Notes
- For uploading assets/metadata, set NFT_STORAGE_API_KEY in your .env. The bridge will upload an example image from the game's Resources path if available.
- For local ETH transfer testing, use Ganache and set PLAYER1/PLAYER2 private keys and addresses in .env.
