const fs = require("fs");
const path = require("path");
const { ethers } = require("ethers");
require("dotenv").config({ path: path.resolve(__dirname, "..", ".env") });

function artifact(name) {
  const artifactPath = path.resolve(
    __dirname,
    "..",
    "artifacts",
    "contracts",
    "GladiatorArenaSystem.sol",
    `${name}.json`
  );
  return JSON.parse(fs.readFileSync(artifactPath, "utf8"));
}

async function deployContract(name, signer, args = []) {
  const compiled = artifact(name);
  const factory = new ethers.ContractFactory(compiled.abi, compiled.bytecode, signer);
  const contract = await factory.deploy(...args);
  await contract.waitForDeployment();
  return contract;
}

async function main() {
  const rpcUrl = process.env.RPC_URL || "http://127.0.0.1:8545";
  const privateKey = process.env.PRIVATE_KEY;
  if (!privateKey) {
    throw new Error("PRIVATE_KEY is required in blockchain/.env");
  }

  const provider = new ethers.JsonRpcProvider(rpcUrl);
  const baseSigner = new ethers.Wallet(privateKey, provider);
  const signer = new ethers.NonceManager(baseSigner);
  const owner = baseSigner.address;

  const currency = await deployContract("ArenaCurrency", signer, [owner]);
  const weapons = await deployContract("WeaponNFT", signer, [owner]);
  const characters = await deployContract("GladiatorCharacter", signer, [owner]);
  const trophy = await deployContract("ArenaTrophy", signer, [owner]);
  const battleArena = await deployContract("BattleArena", signer, [
    owner,
    currency.target,
    trophy.target,
    characters.target
  ]);
  const marketplace = await deployContract("Marketplace", signer, [
    owner,
    currency.target,
    characters.target,
    weapons.target
  ]);

  await (await currency.setOperator(battleArena.target, true)).wait();
  await (await trophy.setOperator(battleArena.target, true)).wait();
  await (await characters.setOperator(battleArena.target, true)).wait();

  const names = [
    "ArenaCurrency",
    "WeaponNFT",
    "GladiatorCharacter",
    "ArenaTrophy",
    "BattleArena",
    "Marketplace"
  ];

  const addresses = {
    ArenaCurrency: currency.target,
    WeaponNFT: weapons.target,
    GladiatorCharacter: characters.target,
    ArenaTrophy: trophy.target,
    BattleArena: battleArena.target,
    Marketplace: marketplace.target
  };

  const abis = {};
  for (const name of names) {
    abis[name] = artifact(name).abi;
  }

  const chainId = Number((await provider.getNetwork()).chainId);
  const output = {
    network: chainId === 31337 ? "localhost" : "custom",
    chainId,
    rpcUrl,
    deployer: owner,
    deployedAt: new Date().toISOString(),
    addresses,
    abis
  };

  const chainOut = path.resolve(__dirname, "..", "deployments.generated.json");
  fs.writeFileSync(chainOut, JSON.stringify(output, null, 2));
  const bridgeOut = path.resolve(__dirname, "..", "..", "node_bridge", "contracts.generated.json");
  try {
    fs.writeFileSync(bridgeOut, JSON.stringify(output, null, 2));
  } catch (error) {
    console.warn(`Could not write ${bridgeOut}: ${error.message}`);
  }

  console.table(addresses);
  console.log(`Generated ${chainOut}`);
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
