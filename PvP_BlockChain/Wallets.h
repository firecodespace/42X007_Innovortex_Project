// Simple global wallet holders for quick wiring between menu and gameplay
#pragma once
#include <string>

extern std::string g_player1_wallet;
extern std::string g_player2_wallet;
// Optional ERC-721 token IDs for on-chain `recordVictory` when the player wins (decimal digits only).
extern std::string g_player1_character_token_id;
extern std::string g_player2_character_token_id;
