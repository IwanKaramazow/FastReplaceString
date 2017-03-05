#!/usr/bin/env bash

if [ ! -f test/small.txt ]; then
    yes "let () = print_endline \"Hey, qué tal?\";" | head -n 10 > test/small.txt
fi

if [ ! -f test/file100mb.txt ]; then
    base64 /dev/urandom | head -c 100000000 > test/file100mb.txt
fi

if [ ! -f test/file10mb.txt ]; then
    base64 /dev/urandom | head -c 10000000 > test/file10mb.txt
fi

cp test/small.txt test/smallc.txt
cp test/small.txt test/smallpython.txt

cp test/file10mb.txt test/file10mbc.txt
cp test/file10mb.txt test/file10mbpython.txt

cp test/file100mb.txt test/file100mbc.txt
cp test/file100mb.txt test/file100mbpython.txt

echo "========================"
echo "small file benchmark: "
echo "c implementation:"
time ./.bin/replacestring test/smallc.txt Hey Hola
echo "python implementation:"
time python ./test/replacestring.py test/smallpython.txt Hey Hola
echo "========================"

echo "========================"
echo "10mb file benchmark: "
echo "c implementation:"
time ./.bin/replacestring test/file10mbc.txt a b
echo "python implementation:"
time python ./test/replacestring.py test/file10mbpython.txt a b
echo "========================"

echo "100mb file benchmark: "
echo "c implementation:"
time ./.bin/replacestring test/file100mbc.txt a b
echo "python implementation:"
time python ./test/replacestring.py test/file100mbpython.txt a b
echo "========================"
