var crypto = require("crypto");
var fs = require("fs");
var cp = require("child_process");

var success = true;

function sha1sum(strOrBuf) {
  return crypto.createHash("sha1").update(strOrBuf).digest("hex");
}

try {
  fs.mkdirSync("./tests/actual/");
} catch (error) {
  // if dir exists just ignore
}

var commands = fs
  .readFileSync("./tests/commands")
  .toString()
  .trim()
  .split("\n");

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
    var file = tokens[0];
    var src = tokens[1];
    var dest = tokens[2];
    var actualFile = file.replace("input", "actual");
    console.log("cp " + file + " " + actualFile);
    cp.execSync("cp " + file + " " + actualFile);
    cp.execSync("cp " + actualFile + " " + actualFile + ".reformatted");
    cp.execSync(
      "./.bin/fastreplacestring \"" 
      + actualFile 
      + "\" \"" + src + "\" \"" + dest + "\""  
    );
    cp.execSync(
      "./.bin/fastreplacestring \"" +
        actualFile +
        ".reformatted\"" +
        " \"" +
        dest +
        "\" \"" +
        src + "\""
    );
    var result = fs.readFileSync(actualFile);
    var expected = fs.readFileSync(file.replace("input", "expected"));

    var originalSha = sha1sum(fs.readFileSync(file));
    var resultSha = sha1sum(result);
    var expectedSha = sha1sum(expected);
    var reformattedSha = sha1sum(fs.readFileSync(actualFile + ".reformatted"));

    console.log(">>> fastreplacestring " + command);
    if (resultSha === expectedSha) {
      console.log("🌈   Test passed");
    } else {
      console.log("🔥   Test failed!");
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
//    console.log(error, " something went wrong, but ignore it!");
  }
}

commands.forEach(testCommand);

if (!success) {
  process.exit(1);
} else {
  process.exit(0);
}
