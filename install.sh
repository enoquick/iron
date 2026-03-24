#!/bin/bash

PREFIX="$1"
set -e
[ "$PREFIX" ] || PREFIX=~/iron
find ./libs/build -name 'libiron*' | while read 
do 
	name="$REPLY"
	compiler=$(dirname "$REPLY" | sed -e 's#./libs/build/###' | sed -e 's#_64.*###'| sed -e 's#_32.*###')  
	installdir="$(echo "$name" | grep 64 > /dev/null && echo "$PREFIX/lib64/$compiler" || echo "$PREFIX/lib/$compiler")"
	echo "$installdir"
	mkdir -p "$installdir"
	install "$name" "$installdir"
done
cp -a includes $PREFIX
