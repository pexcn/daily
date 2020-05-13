#!/bin/bash -e
set -o pipefail

# clone
git clone https://github.com/pexcn/daily.git release -b gh-pages --depth 1 --single-branch --quiet
rm -rf release/.git

# sync
cp -rf dist/* release/

echo "[release]: synced."
