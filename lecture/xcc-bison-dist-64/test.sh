#!/bin/sh

file_name=$1

./xcc test/$file_name.c > $file_name.s
gcc $file_name.s
./a.out > tmp.txt
./a.out

gcc test/$file_name.c
./a.out > ans.txt

if diff tmp.txt ans.txt > /dev/null ; then
  echo "✅ : PASSED"
else
  echo "❌ : FAILED"
fi

rm tmp.txt ans.txt a.out