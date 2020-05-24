#!/bin/bash -e
set -o pipefail

cd tools/ip-dedup
make STANDALONE=1 CFLAGS_EXTRA="-s -Wl,--build-id=none" --silent
cd ../..

# https://github.com/afpd/netaggregate/pull/1
cd tools/netaggregate
make --silent
cd ../..
