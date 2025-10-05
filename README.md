![WhatsApp Image 2025-10-04 at 23 22 25_3172defc](https://github.com/user-attachments/assets/821cffc0-fa6b-46e6-b90d-8deccbba31cf)

# ⚔️ Gladiator Arena - On-Chain Combat Game

<div align="center">

[![Solidity](https://img.shields.io/badge/Solidity-0.8.20-363636?style=for-the-badge&logo=solidity)](https://soliditylang.org/)
[![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus)](https://isocpp.org/)
[![SFML](https://img.shields.io/badge/SFML-2.6-8CC445?style=for-the-badge&logo=sfml)](https://www.sfml-dev.org/)
[![Ethereum](https://img.shields.io/badge/Ethereum-3C3C3D?style=for-the-badge&logo=ethereum)](https://ethereum.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

**A fully on-chain gladiator fighting game where battles, NFT ownership, and trading happen directly on the blockchain.**

[🎮 Play Demo]([#installation](https://drive.google.com/file/d/1rZs6O1J-gPd5GS4QHuYRJ14zMaHlMJPY/view?usp=sharing)) • [📖 Documentation]([#documentation](https://drive.google.com/file/d/16PswiVje0G_eJIBWfSSS1bRE25wuJjk9/view?usp=sharing))

</div>

---

## 🌟 Overview

**Gladiator Arena** is a revolutionary blockchain-based combat game that combines the performance of native C++/SFML graphics with the security and ownership guarantees of Ethereum smart contracts. Players mint unique gladiator NFTs, engage in verifiable on-chain battles, earn cryptocurrency rewards, and trade weapons in a fully decentralized marketplace.

### 🎯 Key Features

- **🏛️ True Asset Ownership**: Characters and weapons are ERC-721/ERC-1155 NFTs owned by players
- **⚔️ Provably Fair Combat**: Battle outcomes determined by Chainlink VRF (Verifiable Random Functions)
- **💰 Play-to-Earn Economy**: Win battles to earn on-chain currency tokens
- **🛡️ Upgradeable Assets**: Enhance character stats and weapon tiers using earned tokens
- **🤝 Decentralized Trading**: P2P marketplace for buying, selling, and swapping NFTs
- **🎨 Native Performance**: Smooth 60fps gameplay with C++/SFML rendering engine
- **🔐 MetaMask Integration**: Secure wallet connection via WalletConnect protocol

---

## 🎮 Gameplay
graph LR
A[Mint Character NFT] --> B[Equip Weapons]
B --> C[Enter Battle Arena]
C --> D{Battle Resolution
Chainlink VRF}
D -->|Victory| E[Earn Currency Tokens]
D -->|Defeat| F[Gain Experience]
E --> G[Upgrade Stats]
E --> H[Buy New Weapons]
G --> C
H --> B
F --> C

style A fill:#4CAF50
style E fill:#FFD700
style D fill:#2196F3


### Battle Flow

1. **Mint Your Gladiator**: Create a unique character NFT with randomized base stats (Strength, Defense, Speed, Health)
2. **Arm Your Warrior**: Equip weapons from your inventory or purchase from the marketplace
3. **Challenge Opponents**: Initiate battles with other players or AI-controlled gladiators
4. **Verifiable Combat**: Battle outcomes calculated on-chain using Chainlink VRF for fair randomness
5. **Claim Rewards**: Winners receive currency tokens minted directly to their wallet
6. **Upgrade & Trade**: Spend tokens to enhance stats or trade assets on the decentralized marketplace

---

## 🏗️ Architecture

### System Overview

graph TB
subgraph "Client Layer"
A[C++ Game Client
SFML Graphics]
B[WalletConnect SDK]
end

subgraph "Wallet Layer"

end

subgraph "Blockchain Layer"
    D[Character NFT Contract<br/>ERC-721]
    E[Weapon NFT Contract<br/>ERC-1155]
    F[Battle Arena Contract]
    G[Currency Token<br/>ERC-20]
    H[Marketplace Contract]
    I[Chainlink VRF Oracle]
end

A <-->|QR Code Session| B
B <-->|Sign Transactions| C
C -->|Submit TX| D
C -->|Submit TX| E
C -->|Submit TX| F
C -->|Submit TX| H
F <-->|Request Randomness| I
F -->|Mint Rewards| G
D -.->|Owns| G
E -.->|Holds| D

style A fill:#00599C
style C fill:#F6851B
style F fill:#627EEA
style I fill:#375BD2


### Smart Contract Architecture

| Contract | Standard | Purpose |
|----------|----------|---------|
| **GladiatorCharacter** | ERC-721 | Unique fighter NFTs with upgradeable stats |
| **WeaponNFT** | ERC-1155 | Semi-fungible weapons with rarity tiers |
| **BattleArena** | Custom | Combat resolution with VRF integration |
| **ArenaCurrency** | ERC-20 | Fungible reward tokens for upgrades |
| **Marketplace** | Custom | P2P trading and fixed-price listings |

---

## 🚀 Installation

### Prerequisites

System Requirements
C++17 compiler (GCC 9+, Clang 10+, or MSVC 2019+)

SFML 2.6+

Node.js 18+
### Clone & Build
