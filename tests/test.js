var crypto = require("crypto");
var fs = require("fs");
var cp = require("child_process");
var path = require("path");

var success = true;

var commands = [
  "basic reason ocaml",
  "nothing house church",
  "shorten rank-2 Mac",
  "lengthen OCaml Reason",
  "ocamlopt Iwan Jordan",
  "multiple Tree BinaryTree",
  "emoji 🌈  ✅",
  "chinese 樹 房子"
];

var executableCommand = path.join(".", ".bin", "fastreplacestring.exe");

function sha1sum(strOrBuf) {
  return crypto.createHash("sha1").update(strOrBuf).digest("hex");
}

try {
  fs.mkdirSync("./tests/actual/");
} catch (error) {
  // if dir exists just ignore
}

function testCommand(command) {
  try {
    var tokens = command.trim().split(" ").filter(function(s) {
      return s.length > 0;
    });
    if (tokens.length !== 3) {
      console.log("Command should be of form: ./tests/input/fileName src dest");
      success = false;
      return;
    }
    var fileName = tokens[0];
    var file = path.join(".", "tests", "input", fileName);
    var src = tokens[1];
    var dest = tokens[2];
    var actualFile = path.join(".", "tests", "actual", fileName); // file.replace("input", "actual");
    var expectedFile = path.join(".", "tests", "expected", fileName);
    var reformattedFile = actualFile + ".reformatted";
    fs.writeFileSync(actualFile, fs.readFileSync(file));
    cp.spawnSync(executableCommand, [actualFile, src, dest]);

    fs.writeFileSync(reformattedFile, fs.readFileSync(file));
    cp.spawnSync(executableCommand, [reformattedFile, dest, src]);

    var result = fs.readFileSync(actualFile);
    var expected = fs.readFileSync( expectedFile); //file.replace("input", "expected"));

    var originalSha = sha1sum(fs.readFileSync(file));
    var resultSha = sha1sum(result);
    var expectedSha = sha1sum(expected);
    var reformattedSha = sha1sum(fs.readFileSync(reformattedFile));

    console.log(">>> fastreplacestring.exe " + command);
    if (resultSha === expectedSha) {
      console.log("🌈   Test passed");
    } else {
      console.log("🔥   Test failed!");
      console.log("expected: ");
      console.log(fs.readFileSync(expectedFile).toString());
      console.log("got: ");
      console.log(fs.readFileSync(actualFile).toString());
      success = false;
    }

    if (originalSha === reformattedSha) {
      console.log("✅   Idempotent");
    } else {
      console.log("😟   Idempotent test failed!");
      success = false;
    }
    console.log("");
  } catch (error) {
    console.log(error);
  }
}

commands.forEach(testCommand);

if(success) {
  process.exit(0);
} else {
  process.exit(1);
}
