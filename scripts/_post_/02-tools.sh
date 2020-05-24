#!/bin/bash -e
set -o pipefail

cd tools/ip-dedup
make clean --silent
cd ../..

cd tools/netaggregate
make clean --silent
cd ../..
