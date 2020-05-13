#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)

cd tools/ip-dedup
make clean --silent
cd $CUR_DIR

cd tools/netaggregate
make clean --silent
cd $CUR_DIR
