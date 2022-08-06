#!/bin/sh

file_name=$1

./xcc test/$file_name.c > $file_name.s
gcc $file_name.s
./a.out

rm a.out