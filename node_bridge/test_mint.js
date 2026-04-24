async function main() {
  const payload = {
    winner: process.env.TEST_WINNER || "0x0000000000000000000000000000000000000001",
    player1: process.env.TEST_PLAYER1 || "0x0000000000000000000000000000000000000001",
    player2: process.env.TEST_PLAYER2 || "0x0000000000000000000000000000000000000002",
    weaponName: "Champion Blade",
    matchId: `manual-test-${Date.now()}`
  };

  const response = await fetch("http://localhost:3000/mintNFT", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload)
  });

  console.log(await response.json());
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
