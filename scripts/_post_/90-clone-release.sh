#!/bin/bash -e
set -o pipefail

git clone https://github.com/pexcn/daily.git release -b gh-pages --depth 1 --single-branch --quiet
rm -rf release/.git
