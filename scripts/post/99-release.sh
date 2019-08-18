#!/bin/bash -e

git clone https://github.com/pexcn/daily.git release -b gh-pages --single-branch --depth 1 --quiet
cd release && rm -rf .git && cd ..
cp -rf dist/* release/

echo "[release]: synced."
