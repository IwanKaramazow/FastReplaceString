# FastReplaceString
Fast native implementation in c(++) of a replace string script as an npm package.
Replaces a string in a file.

## Build status

| Platform  | Status   | Notes |
| --------- | --------- | ----- |
| Linux | [![Build status]( https://g.codefresh.io/api/badges/build?repoOwner=IwanKaramazow&repoName=FastReplaceString&branch=master&pipelineName=FastReplaceString&accountName=IwanKaramazow&type=cf-2)]( https://g.codefresh.io/repositories/IwanKaramazow/FastReplaceString/builds?filter=trigger:build;branch:master;service:58f675e124f7b201009db80c~FastReplaceString) | |
| MacOS | [![Build Status](https://travis-ci.org/IwanKaramazow/FastReplaceString.svg?branch=master)](https://travis-ci.org/IwanKaramazow/FastReplaceString) | |
| Windows | [![Build status](https://ci.appveyor.com/api/projects/status/4968wqmj8o1ktlu5?svg=true)](https://ci.appveyor.com/project/IwanKaramazow/fastreplacestring) | Unicode not yet supported |

## Usage
```
fastreplacestring.exe filename src dest
```
* filename: filename to read
* src: string that will be replaced
* dest: replacement

## Example:

```
echo "let () = print_endline \"Hello world🌍\";" > hello.re

fastreplacestring.exe hello.re world🌍 universe⛄️

cat hello.re
 >>> let () = print_endline "Hello universe⛄️";
```

## Tests:

```
# everything
npm run test

# default tests
node ./tests/test.js

# cross compiling tests
node ./tests/xtest.js
```

## How to contribute:
* clone the repo
* add a new file under `./tests/input/yourFileName`
* add the expected file under `./tests/expected/yourFileName`
* add a command in `./tests/test.js` of form `filename src dest`, e.g. `yourFileName aString aReplacement`
* make changes to `fastreplacestring.cpp` & `npm run build`
* `npm run test`

## Notes on Cross-Compiling

Fastreplacestring.exe has been cross compiled from a Macbook Pro (13" intel i5 early 2015 macOS Sierra 10.12.5)
to the following platforms:

| OS | ARCH | Binary | CI available | 
| -- | ---- | ----- | -- | 
| Linux | 32 | fastreplacestring-linux32.exe | 🚫  | 
| | 64 | fastreplacestring-linux64.exe | ✅ | 
| Windows | 32 | fastreplacestring-win32.exe | ✅ |
| | 64 | fastreplacestring-win64.exe | 🚫 | 
| Darwin | 32 | fastreplacestring-darwin32.exe | 🚫 | 
| | 64 | fastreplacestring-darwin64.exe | ✅ | 

All binaries are located in the `.bin` directory.

