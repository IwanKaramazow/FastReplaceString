# FastReplaceString
Fast native implementation in c of a replace string script as an npm package.
Replaces a string in a file. (supports Unicode)

## Build status

| Platform  | Status   |
| --------- | ---------|
| Linux | [![Build status]( https://g.codefresh.io/api/badges/build?repoOwner=IwanKaramazow&repoName=FastReplaceString&branch=master&pipelineName=FastReplaceString&accountName=IwanKaramazow&type=cf-2)]( https://g.codefresh.io/repositories/IwanKaramazow/FastReplaceString/builds?filter=trigger:build;branch:master;service:58f675e124f7b201009db80c~FastReplaceString) |
| MacOS | [![Build Status](https://travis-ci.org/IwanKaramazow/FastReplaceString.svg?branch=master)](https://travis-ci.org/IwanKaramazow/FastReplaceString) |
| Windows | [![Build status](https://ci.appveyor.com/api/projects/status/4968wqmj8o1ktlu5?svg=true)](https://ci.appveyor.com/project/IwanKaramazow/fastreplacestring) |

## Usage
```
fastreplacestring filename src dest
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
npm run test
# node ./tests/test.js
```

## How to contribute:
* clone the repo
* add a new file under `./tests/input/yourFileName`
* add the expected file under `./tests/expected/yourFileName`
* add a command in `./tests/commands` of form `filename src dest`, e.g. `./tests/input/yourFileName aString aReplacement`
* make changes to `fastreplacestring.c` & `npm run build`
* `npm run test`
