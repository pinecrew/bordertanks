#!/bin/sh

files=$(ls src/*pp)
for f in $files; do
    clang-format -i -style=file $f
done
