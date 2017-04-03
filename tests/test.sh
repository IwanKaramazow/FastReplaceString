#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

mkdir -p $DIR/actual

cp $DIR/input/basic $DIR/actual/basic

$DIR/../.bin/fastreplacestring $DIR/actual/basic reason ocaml

cmp --silent $DIR/expected/basic $DIR/actual/basic && echo "### SUCCESS" && exit 0 || echo "### FAILURE files are different" && exit 1
