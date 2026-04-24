require("dotenv").config();

const express = require("express");
const fs = require("fs");
const morgan = require("morgan");
const path = require("path");
const { ethers } = require("ethers");

let NFTStorage;
let File;
if (process.env.NFT_STORAGE_API_KEY) {
  try {
    ({ NFTStorage, File } = require("nft.storage"));
  } catch (error) {
    console.warn("nft.storage package is unavailable. Metadata will fall back to data URIs.");
  }
}

const PORT = Number(process.env.PORT || 3000);
const RPC_URL = process.env.RPC_URL || "http://127.0.0.1:8545";
const PRIVATE_KEY = process.env.PRIVATE_KEY;
const provider = new ethers.JsonRpcProvider(RPC_URL);

if (!PRIVATE_KEY) {
  console.error("Set PRIVATE_KEY in node_bridge/.env");
  process.exit(1);
}

const ownerWallet = new ethers.Wallet(PRIVATE_KEY, provider);
const generatedContractsPath = fs.existsSync(path.join(__dirname, "contracts.generated.json"))
  ? path.join(__dirname, "contracts.generated.json")
  : path.join(__dirname, "..", "blockchain", "deployments.generated.json");
if (!fs.existsSync(generatedContractsPath)) {
  console.error("Missing generated contract deployment file. Run the blockchain deploy script first.");
  process.exit(1);
}

const generated = JSON.parse(fs.readFileSync(generatedContractsPath, "utf8"));
const { addresses, abis } = generated;

function contract(name, signer = ownerWallet) {
  return new ethers.Contract(addresses[name], abis[name], signer);
}

const battleArena = contract("BattleArena");
const characters = contract("GladiatorCharacter");
const weapons = contract("WeaponNFT");
const marketplace = contract("Marketplace");
const currency = contract("ArenaCurrency");

function parseEvent(receipt, contractInstance, eventName) {
  for (const log of receipt.logs) {
    try {
      const parsed = contractInstance.interface.parseLog(log);
      if (parsed && parsed.name === eventName) {
        return parsed.args;
      }
    } catch (_) {
      // Ignore logs for other contracts.
    }
  }
  return null;
}

async function buildTokenUri(metadata) {
  if (process.env.NFT_STORAGE_API_KEY && NFTStorage && File) {
    try {
      const client = new NFTStorage({ token: process.env.NFT_STORAGE_API_KEY });
      const metaFile = new File([JSON.stringify(metadata)], "metadata.json", {
        type: "application/json"
      });
      const cid = await client.storeBlob(metaFile);
      return `ipfs://${cid}`;
    } catch (error) {
      console.warn("nft.storage upload failed:", error.message);
    }
  }

  return `data:application/json;base64,${Buffer.from(JSON.stringify(metadata)).toString("base64")}`;
}

function buildBattleMetadata({ winner, loser, matchId, weaponName }) {
  return {
    name: `Gladiator Arena Trophy - ${weaponName}`,
    description: `Champion trophy for ${winner} after defeating ${loser} in match ${matchId}.`,
    attributes: [
      { trait_type: "weapon", value: weaponName },
      { trait_type: "match_id", value: matchId },
      { trait_type: "winner", value: winner },
      { trait_type: "loser", value: loser }
    ]
  };
}

function normalizeStats(stats = {}) {
  return {
    strength: Number(stats.strength || 8),
    defense: Number(stats.defense || 8),
    speed: Number(stats.speed || 8),
    health: Number(stats.health || 100)
  };
}

async function managedWalletFromPrivateKey(privateKey) {
  if (!privateKey) {
    throw new Error("A local demo private key is required for marketplace actions.");
  }
  return new ethers.NonceManager(new ethers.Wallet(privateKey, provider));
}

const app = express();
app.use(express.json());
app.use(morgan("dev"));

app.get("/health", async (_req, res) => {
  const network = await provider.getNetwork();
  res.json({
    status: "ok",
    rpcUrl: RPC_URL,
    chainId: Number(network.chainId),
    owner: ownerWallet.address,
    contracts: addresses
  });
});

app.get("/contracts", (_req, res) => {
  res.json({ addresses });
});

app.post("/mintNFT", async (req, res) => {
  try {
    const {
      winner,
      weaponName,
      matchId,
      player1,
      player2,
      winnerCharacterId = 0
    } = req.body;

    if (!winner || !weaponName || !matchId || !player1 || !player2) {
      return res.status(400).json({
        status: "error",
        error: "winner, weaponName, matchId, player1 and player2 are required"
      });
    }

    const loser = winner.toLowerCase() === player1.toLowerCase() ? player2 : player1;
    const tokenUri = await buildTokenUri(
      buildBattleMetadata({ winner, loser, matchId, weaponName })
    );

    const tx = await battleArena.settleBattle(
      winner,
      loser,
      matchId,
      tokenUri,
      BigInt(winnerCharacterId || 0)
    );
    const receipt = await tx.wait();
    const settled = parseEvent(receipt, battleArena, "BattleSettled");

    return res.json({
      status: "success",
      battleTxHash: tx.hash,
      mintTxHash: tx.hash,
      tokenId: settled ? settled.trophyId.toString() : null,
      rewardAmount: settled ? settled.rewardAmountPaid.toString() : null
    });
  } catch (error) {
    console.error("mintNFT error", error);
    return res.status(500).json({ status: "error", error: error.message });
  }
});

app.post("/characters/mint", async (req, res) => {
  try {
    const { to, name, description, stats, image } = req.body;
    if (!to) {
      return res.status(400).json({ status: "error", error: "to is required" });
    }

    const normalized = normalizeStats(stats);
    const tokenUri = await buildTokenUri({
      name: name || "Gladiator Character",
      description: description || "Playable gladiator character",
      image: image || null,
      attributes: [
        { trait_type: "strength", value: normalized.strength },
        { trait_type: "defense", value: normalized.defense },
        { trait_type: "speed", value: normalized.speed },
        { trait_type: "health", value: normalized.health }
      ]
    });

    const tx = await characters.mintCharacter(
      to,
      tokenUri,
      normalized.strength,
      normalized.defense,
      normalized.speed,
      normalized.health
    );
    const receipt = await tx.wait();
    const minted = parseEvent(receipt, characters, "CharacterMinted");

    res.json({
      status: "success",
      txHash: tx.hash,
      tokenId: minted ? minted.tokenId.toString() : null
    });
  } catch (error) {
    console.error("characters/mint error", error);
    res.status(500).json({ status: "error", error: error.message });
  }
});

app.post("/characters/upgrade", async (req, res) => {
  try {
    const {
      tokenId,
      strengthDelta = 0,
      defenseDelta = 0,
      speedDelta = 0,
      healthDelta = 0
    } = req.body;

    if (!tokenId) {
      return res.status(400).json({ status: "error", error: "tokenId is required" });
    }

    const tx = await characters.applyUpgrade(
      BigInt(tokenId),
      Number(strengthDelta),
      Number(defenseDelta),
      Number(speedDelta),
      Number(healthDelta)
    );
    await tx.wait();

    res.json({ status: "success", txHash: tx.hash });
  } catch (error) {
    console.error("characters/upgrade error", error);
    res.status(500).json({ status: "error", error: error.message });
  }
});

app.post("/currency/mint", async (req, res) => {
  try {
    const { to, amount } = req.body;
    if (!to || !amount) {
      return res.status(400).json({ status: "error", error: "to and amount are required" });
    }

    const tx = await currency.mint(to, ethers.parseEther(String(amount)));
    await tx.wait();

    res.json({ status: "success", txHash: tx.hash });
  } catch (error) {
    console.error("currency/mint error", error);
    res.status(500).json({ status: "error", error: error.message });
  }
});

app.post("/weapons/mint", async (req, res) => {
  try {
    const { to, weaponId, amount = 1 } = req.body;
    if (!to || !weaponId) {
      return res.status(400).json({ status: "error", error: "to and weaponId are required" });
    }

    const tx = await weapons.mintTo(to, BigInt(weaponId), BigInt(amount));
    await tx.wait();

    res.json({ status: "success", txHash: tx.hash });
  } catch (error) {
    console.error("weapons/mint error", error);
    res.status(500).json({ status: "error", error: error.message });
  }
});

app.post("/marketplace/list-character", async (req, res) => {
  try {
    const { sellerPrivateKey, tokenId, price } = req.body;
    const seller = await managedWalletFromPrivateKey(sellerPrivateKey);
    const sellerCharacters = contract("GladiatorCharacter", seller);
    const sellerMarketplace = contract("Marketplace", seller);

    await (await sellerCharacters.approve(addresses.Marketplace, BigInt(tokenId))).wait();
    const tx = await sellerMarketplace.listCharacter(BigInt(tokenId), ethers.parseEther(String(price)));
    const receipt = await tx.wait();
    const event = parseEvent(receipt, sellerMarketplace, "ListingCreated");

    res.json({
      status: "success",
      txHash: tx.hash,
      listingId: event ? event.listingId.toString() : null
    });
  } catch (error) {
    console.error("marketplace/list-character error", error);
    res.status(500).json({ status: "error", error: error.message });
  }
});

app.post("/marketplace/list-weapon", async (req, res) => {
  try {
    const { sellerPrivateKey, weaponId, amount = 1, price } = req.body;
    const seller = await managedWalletFromPrivateKey(sellerPrivateKey);
    const sellerWeapons = contract("WeaponNFT", seller);
    const sellerMarketplace = contract("Marketplace", seller);

    await (await sellerWeapons.setApprovalForAll(addresses.Marketplace, true)).wait();
    const tx = await sellerMarketplace.listWeapon(
      BigInt(weaponId),
      BigInt(amount),
      ethers.parseEther(String(price))
    );
    const receipt = await tx.wait();
    const event = parseEvent(receipt, sellerMarketplace, "ListingCreated");

    res.json({
      status: "success",
      txHash: tx.hash,
      listingId: event ? event.listingId.toString() : null
    });
  } catch (error) {
    console.error("marketplace/list-weapon error", error);
    res.status(500).json({ status: "error", error: error.message });
  }
});

app.post("/marketplace/buy", async (req, res) => {
  try {
    const { buyerPrivateKey, listingId } = req.body;
    const buyer = await managedWalletFromPrivateKey(buyerPrivateKey);
    const buyerMarketplace = contract("Marketplace", buyer);
    const buyerCurrency = contract("ArenaCurrency", buyer);

    const listing = await marketplace.listings(BigInt(listingId));
    await (await buyerCurrency.approve(addresses.Marketplace, listing.price)).wait();

    const tx = await buyerMarketplace.buy(BigInt(listingId));
    await tx.wait();

    res.json({ status: "success", txHash: tx.hash });
  } catch (error) {
    console.error("marketplace/buy error", error);
    res.status(500).json({ status: "error", error: error.message });
  }
});

app.listen(PORT, () => {
  console.log(`Bridge running on http://localhost:${PORT}`);
  console.log("Loaded contracts:");
  console.table(addresses);
});
