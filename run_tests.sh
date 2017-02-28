#!/bin/bash

if (( $# < 1 )); then
    echo "Need to pass an program as an argument"
    exit 1
fi

# remove this script from the args

#set -e

stamp=$(date +"%Y-%m-%d_%H-%M-%S")
tmpdir="test_$stamp"
cp -a test "$tmpdir"

cd "$tmpdir"

python test.py "$@"

cd ..

#rm -rf "$tmpdir"
