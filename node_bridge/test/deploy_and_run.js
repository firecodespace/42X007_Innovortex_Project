const fs = require('fs');
const path = require('path');
const { ethers } = require('ethers');
const fetch = require('node-fetch');

async function main() {
  const RPC_URL = process.env.RPC_URL || 'http://127.0.0.1:8545';
  const PRIVATE_KEY = process.env.PRIVATE_KEY; // account used to deploy
  if (!PRIVATE_KEY) throw new Error('Set PRIVATE_KEY env var for deployer');

  const provider = new ethers.JsonRpcProvider(RPC_URL);
  const wallet = new ethers.Wallet(PRIVATE_KEY, provider);

  const abiJson = JSON.parse(fs.readFileSync(path.join(__dirname, '..', 'GameNFT.json')));
  const abi = abiJson.abi || abiJson;
  // We need bytecode to deploy. For quick testing, assume a compiled artifact exists at ../GameNFT.bin
  const binPath = path.join(__dirname, '..', 'GameNFT.bin');
  if (!fs.existsSync(binPath)) throw new Error('GameNFT.bin not found. Compile contract and place bytecode in node_bridge/GameNFT.bin');
  const bytecode = fs.readFileSync(binPath, 'utf8').trim();

  const factory = new ethers.ContractFactory(abi, bytecode, wallet);
  console.log('Deploying GameNFT...');
  const contract = await factory.deploy();
  console.log('Tx:', contract.deployTransaction.hash);
  await contract.waitForDeployment();
  console.log('Deployed at', contract.target);

  // Call the bridge with a test POST
  const payload = {
    winner: '0x' + wallet.address.slice(2),
    player1: '0x' + wallet.address.slice(2),
    player2: '0x000000000000000000000000000000000000dead',
    weaponName: 'Sword_02',
    matchId: 'auto-test'
  };

  console.log('POSTing to bridge...');
  const res = await fetch('http://localhost:3000/mintNFT', { method: 'POST', headers: {'Content-Type':'application/json'}, body: JSON.stringify(payload) });
  const j = await res.json();
  console.log('Bridge response:', j);
}

main().catch(console.error);
