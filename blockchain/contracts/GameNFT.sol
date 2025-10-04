// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

import "@openzeppelin/contracts/token/ERC721/ERC721.sol";

contract GameNFT is ERC721 {
    uint256 public nextId;

    constructor() ERC721("GameNFT", "GNFT") {}

    function mint(address to) external {
        _safeMint(to, nextId);
        nextId++;
    }
}
