const fetch = require('node-fetch');

async function test() {
  const payload = {
    winner: '0xPLAYER1_ADDRESS_FROM_GANACHE',
    player1: '0xPLAYER1_ADDRESS_FROM_GANACHE',
    player2: '0xPLAYER2_ADDRESS_FROM_GANACHE',
    weaponName: 'Sword_02',
    matchId: 'test-match-1234'
  };

  const res = await fetch('http://localhost:3000/mintNFT', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(payload)
  });

  const json = await res.json();
  console.log('Response:', json);
}

test().catch(console.error);
