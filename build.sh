#!/usr/bin/env bash

# exit on errors
set -e

# change to directory of the script
cd "$(dirname "$0")"

# build
docker build -t serpiente .

# extract the result binary
CONTAINER=$(docker create serpiente)
docker cp "$CONTAINER:/code/serpiente" .

# extract the allegro so library too so it doesn't have to be installed
docker cp "$CONTAINER:/code/liballeg.so.4.4" .

# remove the container
docker rm "$CONTAINER"
