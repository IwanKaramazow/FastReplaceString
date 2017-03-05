# FastReplaceString
Fast native implementation in c of a replace string script as an npm package.
Replaces a (wide) string in a file.

## Usage
```
replacestring filename src dest
```
* filename: filename to read
* src: string that will be replaced
* dest: replacement


## Example:

```
echo "let () = print_endline \"Hello world🌍\";" > hello.re

replacestring hello.re world🌍 universe⛄️

cat hello.re
 >>> let () = print_endline "Hello universe⛄️";
```

## Acknowledgements
The core replace algorithm - the heavy lifting - has been adopted from http://creativeandcritical.net/downloads/wreplacebench.c, written by Laird Shaw contactable @ http://creativeandcritical.net/contact.
