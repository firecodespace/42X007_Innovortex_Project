const express = require("express");
const { ethers } = require("ethers");
const fs = require("fs");

// Ganache RPC
const provider = new ethers.JsonRpcProvider("http://127.0.0.1:8545");

// Replace with one Ganache private key
const privateKey = "0x0c10c70b79ac42edaf943034a4e4b9a549e80210cf6964623947dce08eb2f965"; 
const wallet = new ethers.Wallet(privateKey, provider);

// Load ABI (compiled JSON from Hardhat/Remix)
const abi = JSON.parse(fs.readFileSync("GameNFT.json")).abi;
const contractAddress = "DEPLOYED_CONTRACT_ADDRESS"; // from deploy
const contract = new ethers.Contract(contractAddress, abi, wallet);

const app = express();
app.use(express.json());

// Endpoint game will call
app.post("/win", async (req, res) => {
    try {
        const { playerAddress } = req.body;
        const tx = await contract.mint(playerAddress);
        await tx.wait();
        res.json({ status: "success", txHash: tx.hash });
    } catch (err) {
        console.error(err);
        res.status(500).json({ status: "error", error: err.message });
    }
});

app.listen(3000, () => console.log("Bridge running on http://localhost:3000"));
