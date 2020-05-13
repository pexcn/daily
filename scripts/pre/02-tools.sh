#!/bin/bash -e
set -o pipefail

cd tools/ip-dedup
make STANDALONE=1 CFLAGS_EXTRA="-s -Wl,--build-id=none" --silent

cd tools/netaggregate
make --silent
