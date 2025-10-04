Usage: Ganache wallets for game players

The game uses `PvP_BlockChain/Wallets.json` to determine the Ethereum wallet address for each player at runtime.

By default this file contains example Ganache addresses. To use your own Ganache instance:

1. Start Ganache (UI or CLI). The local RPC should be at http://127.0.0.1:8545 by default.
2. Copy one of the account addresses from Ganache's Accounts list (e.g. the first two addresses).
3. Edit `PvP_BlockChain/Wallets.json` and set `player1` and `player2` to those addresses.

Example:
{
  "player1": "0x627306090abaB3A6e1400e9345bC60c78a8BEf57",
  "player2": "0xf17f52151EbEF6C7334FAD080c5704D77216b732"
}

Notes:
- The Node bridge and Hardhat/ganache must use compatible networks (RPC URL and accounts). In this project the bridge connects to Ganache at http://127.0.0.1:8545 by default.
- If you change Ganache accounts, ensure the bridge's private key in `node_bridge/bridge.js` matches an unlocked/prefunded account in Ganache (or change it to one of the Ganache private keys).
- After updating `Wallets.json`, restart the game so it reloads the addresses at runtime.
