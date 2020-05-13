#!/bin/bash -e
set -o pipefail

# avoid losing files when scripts execute failed
git clone https://github.com/pexcn/daily.git release -b gh-pages --depth 1 --single-branch --quiet
rm -rf release/.git
cp -rf dist/* release/

echo "[release]: synced."
