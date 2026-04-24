const { spawnSync } = require("child_process");
const path = require("path");

async function main() {
  const blockchainDir = path.resolve(__dirname, "..", "..", "blockchain");
  const deploy = spawnSync("npm", ["run", "deploy:local"], {
    cwd: blockchainDir,
    stdio: "inherit",
    shell: true
  });

  if (deploy.status !== 0) {
    throw new Error("Contract deployment failed.");
  }

  const response = await fetch("http://localhost:3000/health");
  console.log(await response.json());
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
