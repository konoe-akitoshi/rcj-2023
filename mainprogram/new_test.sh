#!/bin/bash

cd "$(dirname "$0")" || exit 1

if [ $# != 1 ]; then
    echo "argument count must be 1"
    exit 1
fi

DIRECTORY="Tests/$1"

mkdir -p "$DIRECTORY"

ln -sfnv ../../Program/components "$DIRECTORY/components"
ln -sfnv ../../Program/types      "$DIRECTORY/types"
ln -sfnv ../../Program/pin.hpp    "$DIRECTORY/pin.hpp"

echo "void setup() {" >> "$DIRECTORY/$1.ino"
echo "}"              >> "$DIRECTORY/$1.ino"
echo ""               >> "$DIRECTORY/$1.ino"
echo "void loop() {"  >> "$DIRECTORY/$1.ino"
echo "}"              >> "$DIRECTORY/$1.ino"

echo "#ifdef PREVENT_SORCING_FROM_ARDUINO" >> "$DIRECTORY/$1.cpp"
echo "#ifdef LOCAL_INCLUDE"                >> "$DIRECTORY/$1.cpp"
echo "#else"                               >> "$DIRECTORY/$1.cpp"
echo "#endif"                              >> "$DIRECTORY/$1.cpp"
echo ""                                    >> "$DIRECTORY/$1.cpp"
echo "void setup() {"                      >> "$DIRECTORY/$1.cpp"
echo "}"                                   >> "$DIRECTORY/$1.cpp"
echo ""                                    >> "$DIRECTORY/$1.cpp"
echo "void loop() {"                       >> "$DIRECTORY/$1.cpp"
echo "}"                                   >> "$DIRECTORY/$1.cpp"
echo ""                                    >> "$DIRECTORY/$1.cpp"
echo "#endif"                              >> "$DIRECTORY/$1.cpp"
