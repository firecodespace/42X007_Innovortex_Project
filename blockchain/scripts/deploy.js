const fs = require('fs');

async function main() {
  const [deployer] = await ethers.getSigners();
  console.log('Deploying contracts with account:', deployer.address);

  const GameNFT = await ethers.getContractFactory('GameNFT');
  const nft = await GameNFT.deploy();
  // Support different ethers/hardhat versions
  if (typeof nft.waitForDeployment === 'function') {
    await nft.waitForDeployment();
  } else if (typeof nft.deployed === 'function') {
    await nft.deployed();
  }

  const address = nft.target || nft.address;
  console.log('GameNFT deployed to:', address);
  fs.writeFileSync('deployed-address.txt', address.toString());
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
