// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

import "@openzeppelin/contracts/access/Ownable.sol";
import "@openzeppelin/contracts/token/ERC1155/ERC1155.sol";
import "@openzeppelin/contracts/token/ERC1155/utils/ERC1155Holder.sol";
import "@openzeppelin/contracts/token/ERC20/ERC20.sol";
import "@openzeppelin/contracts/token/ERC721/ERC721.sol";
import "@openzeppelin/contracts/token/ERC721/extensions/ERC721URIStorage.sol";
import "@openzeppelin/contracts/token/ERC721/utils/ERC721Holder.sol";

abstract contract ArenaAuthorizable is Ownable {
    mapping(address => bool) public operators;

    error NotAuthorized();

    constructor(address initialOwner) Ownable(initialOwner) {}

    modifier onlyAuthorized() {
        if (owner() != _msgSender() && !operators[_msgSender()]) {
            revert NotAuthorized();
        }
        _;
    }

    function setOperator(address operator, bool allowed) external onlyOwner {
        operators[operator] = allowed;
    }
}

contract ArenaCurrency is ERC20, ArenaAuthorizable {
    constructor(address initialOwner) ERC20("Arena Currency", "AGLD") ArenaAuthorizable(initialOwner) {}

    function mint(address to, uint256 amount) external onlyAuthorized {
        _mint(to, amount);
    }
}

contract WeaponNFT is ERC1155, ArenaAuthorizable {
    struct WeaponDefinition {
        string name;
        uint256 price;
        uint256 attackBonus;
        bool exists;
    }

    mapping(uint256 => WeaponDefinition) public definitions;

    event WeaponDefined(uint256 indexed weaponId, string name, uint256 price, uint256 attackBonus);
    event WeaponMinted(address indexed to, uint256 indexed weaponId, uint256 amount);

    constructor(address initialOwner) ERC1155("") ArenaAuthorizable(initialOwner) {
        _defineWeapon(1, "Iron Sword", 50 ether, 5);
        _defineWeapon(2, "Aegis Shield", 40 ether, 2);
        _defineWeapon(3, "Storm Spear", 75 ether, 8);
    }

    function defineWeapon(
        uint256 weaponId,
        string calldata name,
        uint256 price,
        uint256 attackBonus
    ) external onlyAuthorized {
        _defineWeapon(weaponId, name, price, attackBonus);
    }

    function mintTo(address to, uint256 weaponId, uint256 amount) external onlyAuthorized {
        require(definitions[weaponId].exists, "weapon undefined");
        _mint(to, weaponId, amount, "");
        emit WeaponMinted(to, weaponId, amount);
    }

    function _defineWeapon(
        uint256 weaponId,
        string memory name,
        uint256 price,
        uint256 attackBonus
    ) internal {
        definitions[weaponId] = WeaponDefinition({
            name: name,
            price: price,
            attackBonus: attackBonus,
            exists: true
        });
        emit WeaponDefined(weaponId, name, price, attackBonus);
    }
}

contract GladiatorCharacter is ERC721URIStorage, ArenaAuthorizable {
    struct Stats {
        uint16 strength;
        uint16 defense;
        uint16 speed;
        uint16 health;
        uint16 level;
        uint16 wins;
    }

    uint256 public nextTokenId = 1;
    mapping(uint256 => Stats) public stats;

    event CharacterMinted(address indexed to, uint256 indexed tokenId);
    event CharacterUpgraded(uint256 indexed tokenId, uint16 strength, uint16 defense, uint16 speed, uint16 health);
    event CharacterVictory(uint256 indexed tokenId, uint16 wins, uint16 level);

    constructor(address initialOwner) ERC721("Gladiator Character", "GLAD") ArenaAuthorizable(initialOwner) {}

    function mintCharacter(
        address to,
        string calldata tokenUri,
        uint16 strength,
        uint16 defense,
        uint16 speed,
        uint16 health
    ) external onlyAuthorized returns (uint256 tokenId) {
        tokenId = nextTokenId++;
        _safeMint(to, tokenId);
        _setTokenURI(tokenId, tokenUri);
        stats[tokenId] = Stats(strength, defense, speed, health, 1, 0);
        emit CharacterMinted(to, tokenId);
    }

    function applyUpgrade(
        uint256 tokenId,
        uint16 strengthDelta,
        uint16 defenseDelta,
        uint16 speedDelta,
        uint16 healthDelta
    ) external onlyAuthorized {
        require(_ownerOf(tokenId) != address(0), "character missing");
        Stats storage current = stats[tokenId];
        current.strength += strengthDelta;
        current.defense += defenseDelta;
        current.speed += speedDelta;
        current.health += healthDelta;
        current.level += 1;
        emit CharacterUpgraded(tokenId, current.strength, current.defense, current.speed, current.health);
    }

    function recordVictory(uint256 tokenId) external onlyAuthorized {
        require(_ownerOf(tokenId) != address(0), "character missing");
        Stats storage current = stats[tokenId];
        current.wins += 1;
        current.level += 1;
        current.strength += 1;
        current.health += 5;
        emit CharacterVictory(tokenId, current.wins, current.level);
    }
}

contract ArenaTrophy is ERC721URIStorage, ArenaAuthorizable {
    uint256 public nextTokenId = 1;

    event TrophyMinted(address indexed to, uint256 indexed tokenId, string matchId);

    constructor(address initialOwner) ERC721("Arena Trophy", "TROPHY") ArenaAuthorizable(initialOwner) {}

    function mintTrophy(address to, string calldata tokenUri, string calldata matchId)
        external
        onlyAuthorized
        returns (uint256 tokenId)
    {
        tokenId = nextTokenId++;
        _safeMint(to, tokenId);
        _setTokenURI(tokenId, tokenUri);
        emit TrophyMinted(to, tokenId, matchId);
    }
}

contract BattleArena is ArenaAuthorizable {
    ArenaCurrency public immutable currency;
    ArenaTrophy public immutable trophy;
    GladiatorCharacter public immutable characters;
    uint256 public rewardAmount = 100 ether;

    event BattleSettled(
        string indexed matchId,
        address indexed winner,
        address indexed loser,
        uint256 rewardAmountPaid,
        uint256 trophyId
    );

    constructor(
        address initialOwner,
        ArenaCurrency currency_,
        ArenaTrophy trophy_,
        GladiatorCharacter characters_
    ) ArenaAuthorizable(initialOwner) {
        currency = currency_;
        trophy = trophy_;
        characters = characters_;
    }

    function setRewardAmount(uint256 amount) external onlyAuthorized {
        rewardAmount = amount;
    }

    function settleBattle(
        address winner,
        address loser,
        string calldata matchId,
        string calldata rewardUri,
        uint256 winnerCharacterId
    ) external onlyAuthorized returns (uint256 trophyId) {
        currency.mint(winner, rewardAmount);
        if (winnerCharacterId != 0) {
            characters.recordVictory(winnerCharacterId);
        }
        trophyId = trophy.mintTrophy(winner, rewardUri, matchId);
        emit BattleSettled(matchId, winner, loser, rewardAmount, trophyId);
    }
}

contract Marketplace is ArenaAuthorizable, ERC721Holder, ERC1155Holder {
    enum AssetKind {
        Character,
        Weapon
    }

    struct Listing {
        uint256 id;
        AssetKind kind;
        address seller;
        uint256 tokenId;
        uint256 amount;
        uint256 price;
        bool active;
    }

    uint256 public nextListingId = 1;
    mapping(uint256 => Listing) public listings;

    ArenaCurrency public immutable currency;
    GladiatorCharacter public immutable characters;
    WeaponNFT public immutable weapons;

    event ListingCreated(
        uint256 indexed listingId,
        AssetKind indexed kind,
        address indexed seller,
        uint256 tokenId,
        uint256 amount,
        uint256 price
    );
    event ListingSold(uint256 indexed listingId, address indexed buyer);
    event ListingCancelled(uint256 indexed listingId);

    constructor(
        address initialOwner,
        ArenaCurrency currency_,
        GladiatorCharacter characters_,
        WeaponNFT weapons_
    ) ArenaAuthorizable(initialOwner) {
        currency = currency_;
        characters = characters_;
        weapons = weapons_;
    }

    function listCharacter(uint256 tokenId, uint256 price) external returns (uint256 listingId) {
        require(characters.ownerOf(tokenId) == _msgSender(), "not owner");
        require(price > 0, "price required");

        characters.safeTransferFrom(_msgSender(), address(this), tokenId);
        listingId = _createListing(AssetKind.Character, _msgSender(), tokenId, 1, price);
    }

    function listWeapon(uint256 tokenId, uint256 amount, uint256 price) external returns (uint256 listingId) {
        require(amount > 0, "amount required");
        require(price > 0, "price required");

        weapons.safeTransferFrom(_msgSender(), address(this), tokenId, amount, "");
        listingId = _createListing(AssetKind.Weapon, _msgSender(), tokenId, amount, price);
    }

    function buy(uint256 listingId) external {
        Listing storage listing = listings[listingId];
        require(listing.active, "listing inactive");

        listing.active = false;
        require(currency.transferFrom(_msgSender(), listing.seller, listing.price), "payment failed");

        if (listing.kind == AssetKind.Character) {
            characters.safeTransferFrom(address(this), _msgSender(), listing.tokenId);
        } else {
            weapons.safeTransferFrom(address(this), _msgSender(), listing.tokenId, listing.amount, "");
        }

        emit ListingSold(listingId, _msgSender());
    }

    function cancelListing(uint256 listingId) external {
        Listing storage listing = listings[listingId];
        require(listing.active, "listing inactive");
        require(listing.seller == _msgSender() || owner() == _msgSender(), "not seller");

        listing.active = false;

        if (listing.kind == AssetKind.Character) {
            characters.safeTransferFrom(address(this), listing.seller, listing.tokenId);
        } else {
            weapons.safeTransferFrom(address(this), listing.seller, listing.tokenId, listing.amount, "");
        }

        emit ListingCancelled(listingId);
    }

    function supportsInterface(bytes4 interfaceId)
        public
        view
        override(ERC1155Holder)
        returns (bool)
    {
        return super.supportsInterface(interfaceId);
    }

    function _createListing(
        AssetKind kind,
        address seller,
        uint256 tokenId,
        uint256 amount,
        uint256 price
    ) internal returns (uint256 listingId) {
        listingId = nextListingId++;
        listings[listingId] = Listing({
            id: listingId,
            kind: kind,
            seller: seller,
            tokenId: tokenId,
            amount: amount,
            price: price,
            active: true
        });
        emit ListingCreated(listingId, kind, seller, tokenId, amount, price);
    }
}
