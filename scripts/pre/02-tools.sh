#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)

cd tools/ip-dedup
make STANDALONE=1 CFLAGS_EXTRA="-s -Wl,--build-id=none" --silent
cd $CUR_DIR

cd tools/netaggregate
make --silent
cd $CUR_DIR
