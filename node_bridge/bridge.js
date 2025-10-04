const express = require("express");
const { ethers } = require("ethers");
const fs = require("fs");

// Ganache RPC
const provider = new ethers.JsonRpcProvider("http://127.0.0.1:8545");

// Bridge account (used to call contract.mint).
// IMPORTANT: Do NOT hardcode private keys. Provide BRIDGE_PRIVATE_KEY as an environment variable.
const bridgePrivateKey = process.env.BRIDGE_PRIVATE_KEY;
if (!bridgePrivateKey) {
    console.error('\nERROR: BRIDGE_PRIVATE_KEY environment variable is not set.\n' +
        'Set BRIDGE_PRIVATE_KEY in your shell or create a local file to load it (do NOT commit secrets).\n');
    process.exit(1);
}
const wallet = new ethers.Wallet(bridgePrivateKey, provider);

// Load ABI (compiled JSON from Hardhat/Remix)
const abi = JSON.parse(fs.readFileSync("GameNFT.json")).abi;
// Try to read a deployed address file from the blockchain folder (created by deploy script)
let contractAddress = null;
try {
    const addr = fs.readFileSync('../blockchain/deployed-address.txt', 'utf8').trim();
    if (addr && addr.startsWith('0x')) contractAddress = addr;
} catch (e) {
    // fallback to local placeholder
    contractAddress = "DEPLOYED_CONTRACT_ADDRESS";
}

// Try to load the game's Wallets.json so we know player addresses (player1/player2)
let gameWallets = null;
try {
    gameWallets = JSON.parse(fs.readFileSync('../PvP_BlockChain/Wallets.json', 'utf8'));
} catch (e) {
    console.warn('Could not read ../PvP_BlockChain/Wallets.json. Make sure it exists and contains player1/player2 addresses.');
}

// Try to load ganache private keys for players (used to sign the 30 ETH transfer)
let ganacheKeys = null;
try {
    ganacheKeys = JSON.parse(fs.readFileSync('ganache_keys.json', 'utf8'));
} catch (e) {
    console.warn('Could not read node_bridge/ganache_keys.json. Create this file with player private keys to enable ETH transfers.');
}

// Normalize ganache keys map to lowercase addresses for robust lookup
let ganacheKeysNormalized = null;
if (ganacheKeys) {
    ganacheKeysNormalized = {};
    for (const k of Object.keys(ganacheKeys)) {
        try {
            const norm = ethers.getAddress(k);
            ganacheKeysNormalized[norm.toLowerCase()] = ganacheKeys[k];
        } catch (e) {
            // skip invalid address keys
            console.warn('Skipping invalid address in ganache_keys.json:', k);
        }
    }
}

let contract = null;
// Only construct the Contract object if a valid hex address is provided to avoid ENS resolution
if (typeof contractAddress === 'string' && contractAddress.startsWith('0x') && contractAddress.length >= 42) {
    contract = new ethers.Contract(contractAddress, abi, wallet);
} else {
    console.warn('Contract address not configured. /win will return an error until contract is deployed and contractAddress is set.');
}

const app = express();
app.use(express.json());

// Endpoint game will call
app.post("/win", async (req, res) => {
    try {
        const payload = req.body || {};
        console.log('Received /win payload:', payload);

        // Accept various legacy keys: winnerAddress or playerAddress or player
        let winnerAddress = payload.winnerAddress || payload.playerAddress || payload.player || null;
        let loserAddress = payload.loserAddress || payload.loser || null;

        // If winnerAddress is missing -> bad request
        if (!winnerAddress) {
            return res.status(400).json({ status: 'error', error: 'winnerAddress required in request body' });
        }

        // If loserAddress not provided, try to infer from gameWallets
        if (!loserAddress && gameWallets) {
            try {
                const p1 = gameWallets.player1 && ethers.getAddress(gameWallets.player1);
                const p2 = gameWallets.player2 && ethers.getAddress(gameWallets.player2);
                const winnerNorm = ethers.getAddress(winnerAddress);
                if (p1 && p1.toLowerCase() === winnerNorm.toLowerCase() && p2) loserAddress = p2;
                else if (p2 && p2.toLowerCase() === winnerNorm.toLowerCase() && p1) loserAddress = p1;
            } catch (e) {
                // ignore and continue; we'll validate below
            }
        }

        // Validate addresses
        try {
            winnerAddress = ethers.getAddress(winnerAddress);
            if (loserAddress) loserAddress = ethers.getAddress(loserAddress);
        } catch (addrErr) {
            console.error('Address validation failed:', addrErr.message);
            return res.status(400).json({ status: 'error', error: 'Invalid Ethereum address provided' });
        }

        if (!contract) {
            console.warn('Contract not configured on bridge. Aborting.');
            return res.status(500).json({ status: "error", error: "Contract not configured on bridge. Set contractAddress in bridge.js after deploying the contract." });
        }

        // If ganacheKeys are provided, find the private key for the loser to sign the transfer
        if (!ganacheKeysNormalized) {
            console.warn('No ganache_keys.json loaded; skipping ETH transfer.');
        } else {
            const lk = loserAddress ? loserAddress.toLowerCase() : null;
            const loserPk = lk && ganacheKeysNormalized[lk];
            if (!loserPk) {
                console.warn('No private key found for loser address; skipping ETH transfer.');
            } else {
                try {
                    const loserWallet = new ethers.Wallet(loserPk, provider);
                    const txValue = ethers.parseEther('30.0');
                    const tx = await loserWallet.sendTransaction({ to: winnerAddress, value: txValue });
                    console.log('Transfer tx sent:', tx.hash);
                    await tx.wait();
                    console.log('Transfer confirmed:', tx.hash);
                } catch (txErr) {
                    console.error('Transfer failed:', txErr);
                    // continue to mint even if transfer fails
                }
            }
        }

        // Mint the NFT to the winner
        const mintTx = await contract.mint(winnerAddress);
        await mintTx.wait();
        res.json({ status: "success", mintTx: mintTx.hash });
    } catch (err) {
        console.error(err);
        res.status(500).json({ status: "error", error: err.message });
    }
});

app.listen(3000, () => console.log("Bridge running on http://localhost:3000"));
