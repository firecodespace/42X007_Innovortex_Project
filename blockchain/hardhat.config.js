require("@nomicfoundation/hardhat-toolbox");
require("dotenv").config();

const { RPC_URL, PRIVATE_KEY, SEPOLIA_RPC_URL, SEPOLIA_PRIVATE_KEY } = process.env;

module.exports = {
  solidity: {
    version: "0.8.24",
    settings: {
      optimizer: {
        enabled: true,
        runs: 200
      }
    }
  },
  networks: {
    hardhat: {},
    localhost: {
      url: RPC_URL || "http://127.0.0.1:8545"
    },
    sepolia: {
      url: SEPOLIA_RPC_URL || RPC_URL || "",
      accounts: SEPOLIA_PRIVATE_KEY ? [SEPOLIA_PRIVATE_KEY] : (PRIVATE_KEY ? [PRIVATE_KEY] : [])
    }
  }
};
