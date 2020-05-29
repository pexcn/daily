#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/ublacklist.XXXXXX)

SRC_URL="https://raw.githubusercontent.com/pexcn/ublacklist/master/ublacklist.txt"
DEST_FILE="dist/ublacklist/ublacklist.txt"

fetch_src() {
  cd $TMP_DIR

  curl -sSL $SRC_URL -o ublacklist.txt

  cd $CUR_DIR
}

gen_list() {
  cd $TMP_DIR

  # remove empty and comment lines
  sed -i -e '/^$/d' -e '/^#/ d' ublacklist.txt
  # convert to ublacklist format
  sed -i -e 's/^/*:\/\/*./' -e 's/$/\/*/' ublacklist.txt

  cd $CUR_DIR
}

copy_dest() {
  install -D -m 644 $TMP_DIR/ublacklist.txt $DEST_FILE
}

clean_up() {
  rm -r $TMP_DIR
  echo "[$(basename $0 .sh)]: done."
}

fetch_src
gen_list
copy_dest
clean_up
