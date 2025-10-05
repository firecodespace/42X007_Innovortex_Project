require('dotenv').config();
const express = require('express');
const morgan = require('morgan');
const fs = require('fs');
const { ethers } = require('ethers');
let NFTStorage, File;
if (process.env.NFT_STORAGE_API_KEY) {
    try {
        ({ NFTStorage, File } = require('nft.storage'));
    } catch (e) {
        console.warn('nft.storage package not available. Install dependencies with npm install');
    }
}

const RPC_URL = process.env.RPC_URL;
const PRIVATE_KEY = process.env.PRIVATE_KEY;
const CONTRACT_ADDRESS = process.env.CONTRACT_ADDRESS;

// Optional ganache player keys (for local ETH transfer)
const PLAYER1_PRIVATE_KEY = process.env.PLAYER1_PRIVATE_KEY;
const PLAYER1_ADDRESS = process.env.PLAYER1_ADDRESS;
const PLAYER2_PRIVATE_KEY = process.env.PLAYER2_PRIVATE_KEY;
const PLAYER2_ADDRESS = process.env.PLAYER2_ADDRESS;

if (!RPC_URL || !PRIVATE_KEY || !CONTRACT_ADDRESS) {
    console.error('Missing environment variables. Copy .env.example to .env and set values.');
    process.exit(1);
}

const provider = new ethers.JsonRpcProvider(RPC_URL);
const wallet = new ethers.Wallet(PRIVATE_KEY, provider);

// Load ABI from compiled JSON (ensure GameNFT.json exists next to bridge.js)
let abi;
try {
    const compiled = JSON.parse(fs.readFileSync('GameNFT.json', 'utf8'));
    abi = compiled.abi || compiled;
} catch (e) {
    console.error('Failed to load GameNFT.json ABI file:', e.message);
    process.exit(1);
}

const contract = new ethers.Contract(CONTRACT_ADDRESS, abi, wallet);

const app = express();
app.use(express.json());
app.use(morgan('dev'));

// /mintNFT endpoint expects { winner, weaponName, matchId }
app.post('/mintNFT', async (req, res) => {
        try {
            // Expect { winner, weaponName, matchId, player1, player2 }
            const { winner, weaponName, matchId, player1, player2 } = req.body;
            if (!winner || !weaponName || !matchId || !player1 || !player2) {
                return res.status(400).json({ status: 'error', error: 'Missing fields. Require winner, weaponName, matchId, player1, player2' });
            }

            // Determine loser address
            const loser = (winner.toLowerCase() === player1.toLowerCase()) ? player2 : player1;

            // Attempt ETH transfer of 30 ETH from loser to winner using loser's private key provided via env (Ganache use-case)
            let transferTxHash = null;
            try {
                // Select appropriate private key from env (only for local testing with Ganache)
                let loserPrivateKey = null;
                if (loser.toLowerCase() === (PLAYER1_ADDRESS || '').toLowerCase()) loserPrivateKey = PLAYER1_PRIVATE_KEY;
                if (loser.toLowerCase() === (PLAYER2_ADDRESS || '').toLowerCase()) loserPrivateKey = PLAYER2_PRIVATE_KEY;

                if (!loserPrivateKey) {
                    console.warn('No private key available for loser address in env; skipping ETH transfer');
                } else {
                    const loserWallet = new ethers.Wallet(loserPrivateKey, provider);
                    const amount = ethers.parseEther('30');
                    console.log(`Transferring 30 ETH from ${loser} to ${winner}...`);
                    const txTransfer = await loserWallet.sendTransaction({ to: winner, value: amount });
                    console.log('Submitted transfer tx:', txTransfer.hash);
                    const receiptTransfer = await txTransfer.wait();
                    transferTxHash = txTransfer.hash;
                    console.log('Transfer mined in block', receiptTransfer.blockNumber);
                }
            } catch (e) {
                console.error('Transfer error:', e);
                // Continue to mint even if transfer fails; include transfer error in response
            }

                // Proceed to mint NFT
                const metadata = {
                    name: `Gladiator Arena Reward - ${weaponName}`,
                    description: `Winner ${winner} of match ${matchId}`,
                    attributes: [{ trait_type: 'weapon', value: weaponName }, { trait_type: 'matchId', value: matchId }]
                };

                let tokenURI = null;
                    // if nft.storage is configured, upload metadata and optional image and get IPFS URL
                    if (process.env.NFT_STORAGE_API_KEY && NFTStorage) {
                        try {
                            const client = new NFTStorage({ token: process.env.NFT_STORAGE_API_KEY });

                            // Optionally upload an image asset first
                            let imageCid = null;
                            const imagePath = process.env.METADATA_IMAGE_PATH || '../PvP_BlockChain/Resources/Images/Background/arena.jpg';
                            if (fs.existsSync(imagePath)) {
                                const imageBuf = fs.readFileSync(imagePath);
                                const file = new File([imageBuf], 'asset.jpg', { type: 'image/jpeg' });
                                const cid = await client.storeBlob(file);
                                imageCid = cid.toString();
                                console.log('Uploaded image to nft.storage ipfs://', imageCid);
                            }

                            // If image uploaded, add image field to metadata as ipfs://<cid>
                            if (imageCid) metadata.image = `ipfs://${imageCid}`;

                            // store metadata JSON
                            const metaFile = new File([JSON.stringify(metadata)], 'metadata.json', { type: 'application/json' });
                            const metaCid = await client.storeBlob(metaFile);
                            tokenURI = `ipfs://${metaCid.toString()}`;
                            console.log('Uploaded metadata to nft.storage, tokenURI=', tokenURI);
                        } catch (e) {
                            console.warn('nft.storage upload failed, falling back to data URI:', e.message);
                        }
                    }

                // Fallback to data URI if not uploaded
                if (!tokenURI) {
                    const metadataStr = JSON.stringify(metadata);
                    const metadataBase64 = Buffer.from(metadataStr).toString('base64');
                    tokenURI = `data:application/json;base64,${metadataBase64}`;
                }

                console.log('Minting NFT to', winner, 'with tokenURI length', tokenURI.length);
                const tx = await contract.mintNFT(winner, tokenURI);
            console.log('Submitted mint tx:', tx.hash);
            const receipt = await tx.wait();
            console.log('Mint mined in block', receipt.blockNumber);

            let tokenId = null;
            try {
                for (const e of receipt.events || []) {
                    if (e.event === 'Minted') {
                        tokenId = e.args && e.args[1] ? e.args[1].toString() : null;
                        break;
                    }
                }
            } catch (e) {
                console.warn('Could not parse Minted event:', e.message);
            }

            return res.json({ status: 'success', transferTxHash, mintTxHash: tx.hash, tokenId });
        } catch (err) {
            console.error('Error in /mintNFT:', err);
            return res.status(500).json({ status: 'error', error: err.message });
        }
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => console.log(`Bridge running on http://localhost:${PORT}`));
