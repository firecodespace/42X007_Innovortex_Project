const { expect } = require("chai");
const { ethers } = require("hardhat");
const { loadFixture } = require("@nomicfoundation/hardhat-network-helpers");

async function deploySystem() {
  const [owner, alice, bob] = await ethers.getSigners();

  const currency = await (await ethers.getContractFactory("ArenaCurrency")).deploy(owner.address);
  const weapons = await (await ethers.getContractFactory("WeaponNFT")).deploy(owner.address);
  const characters = await (await ethers.getContractFactory("GladiatorCharacter")).deploy(owner.address);
  const trophy = await (await ethers.getContractFactory("ArenaTrophy")).deploy(owner.address);
  const battleArena = await (await ethers.getContractFactory("BattleArena")).deploy(
    owner.address,
    await currency.getAddress(),
    await trophy.getAddress(),
    await characters.getAddress()
  );
  const marketplace = await (await ethers.getContractFactory("Marketplace")).deploy(
    owner.address,
    await currency.getAddress(),
    await characters.getAddress(),
    await weapons.getAddress()
  );

  await (await currency.setOperator(await battleArena.getAddress(), true)).wait();
  await (await trophy.setOperator(await battleArena.getAddress(), true)).wait();
  await (await characters.setOperator(await battleArena.getAddress(), true)).wait();

  return { owner, alice, bob, currency, weapons, characters, trophy, battleArena, marketplace };
}

describe("GladiatorArenaSystem", function () {
  it("reverts settleBattle when caller is not authorized", async function () {
    const { battleArena, alice, bob } = await loadFixture(deploySystem);
    await expect(
      battleArena
        .connect(alice)
        .settleBattle(alice.address, bob.address, "m-1", "data:application/json;base64,e30=", 0)
    ).to.be.revertedWithCustomError(battleArena, "NotAuthorized");
  });

  it("settleBattle mints currency and trophy to winner", async function () {
    const { owner, alice, bob, currency, trophy, battleArena } = await loadFixture(deploySystem);
    const reward = ethers.parseEther("100");
    const uri = "data:application/json;base64,e30=";

    await expect(battleArena.connect(owner).settleBattle(alice.address, bob.address, "m-2", uri, 0))
      .to.emit(battleArena, "BattleSettled")
      .withArgs("m-2", alice.address, bob.address, reward, 1n);

    expect(await currency.balanceOf(alice.address)).to.equal(reward);
    expect(await trophy.ownerOf(1)).to.equal(alice.address);
  });

  it("settleBattle with character id records victory stats", async function () {
    const { owner, alice, bob, characters, battleArena } = await loadFixture(deploySystem);
    const uri = "data:application/json;base64,e30=";

    await characters
      .connect(owner)
      .mintCharacter(alice.address, uri, 10, 10, 10, 100);

    const before = await characters.stats(1n);
    expect(before.wins).to.equal(0n);

    await battleArena.connect(owner).settleBattle(alice.address, bob.address, "m-3", uri, 1n);

    const after = await characters.stats(1n);
    expect(after.wins).to.equal(1n);
    expect(after.level).to.be.gt(before.level);
  });

  it("marketplace character listing and buy", async function () {
    const { owner, alice, bob, currency, characters, marketplace } = await loadFixture(deploySystem);
    const uri = "data:application/json;base64,e30=";

    await characters.connect(owner).mintCharacter(alice.address, uri, 5, 5, 5, 50);
    const price = ethers.parseEther("25");
    await currency.connect(owner).mint(bob.address, price);

    await characters.connect(alice).approve(await marketplace.getAddress(), 1n);
    await expect(marketplace.connect(alice).listCharacter(1n, price))
      .to.emit(marketplace, "ListingCreated")
      .withArgs(1n, 0, alice.address, 1n, 1n, price);

    await currency.connect(bob).approve(await marketplace.getAddress(), price);
    await expect(marketplace.connect(bob).buy(1n)).to.emit(marketplace, "ListingSold").withArgs(1n, bob.address);

    expect(await characters.ownerOf(1n)).to.equal(bob.address);
  });

  it("marketplace cancel returns asset to seller", async function () {
    const { owner, alice, currency, characters, marketplace } = await loadFixture(deploySystem);
    const uri = "data:application/json;base64,e30=";

    await characters.connect(owner).mintCharacter(alice.address, uri, 5, 5, 5, 50);
    const price = ethers.parseEther("10");
    await characters.connect(alice).approve(await marketplace.getAddress(), 1n);
    await marketplace.connect(alice).listCharacter(1n, price);

    await marketplace.connect(alice).cancelListing(1n);
    expect(await characters.ownerOf(1n)).to.equal(alice.address);
    expect((await currency.balanceOf(alice.address))).to.equal(0n);
  });
});
