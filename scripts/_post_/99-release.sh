#!/bin/bash -e
set -o pipefail

# avoid losing files when scripts execute failed
cp -rf dist/* release/
