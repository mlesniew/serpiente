#!/usr/bin/env bash

NAME=serpiente

# exit on errors
set -e

# change to directory of the script
cd "$(dirname "$0")"

# build
docker build -t "$NAME" .

# extract the result binary
CONTAINER=$(docker create $NAME)
docker cp "$CONTAINER:/code/$NAME" .

# extract the allegro so library too so it doesn't have to be installed
docker cp "$CONTAINER:/code/liballeg.so.4.4" .

# remove the container
docker rm "$CONTAINER"

echo
echo
echo "  All done, run ./$NAME to play the game."
echo
echo
