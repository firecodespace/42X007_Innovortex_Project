// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import "@openzeppelin/contracts/token/ERC721/extensions/ERC721URIStorage.sol";

contract GameNFT is ERC721URIStorage {
    uint256 public nextId;

    event Minted(address indexed to, uint256 indexed tokenId, string tokenURI);

    constructor() ERC721("GameNFT", "GNFT") {
        nextId = 1; // start IDs at 1
    }

    /// @notice Mint a new NFT to `to` with metadata `tokenURI`.
    /// @dev Emits {Minted} and returns tokenId.
    function mintNFT(address to, string memory tokenURI) external returns (uint256) {
        uint256 tokenId = nextId;
        _safeMint(to, tokenId);
        _setTokenURI(tokenId, tokenURI);
        nextId++;
        emit Minted(to, tokenId, tokenURI);
        return tokenId;
    }
}
