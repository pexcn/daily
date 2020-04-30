#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/ublacklist.XXXXXX)

DIST_FILE="dist/ublacklist/ublacklist.txt"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

UBLACKLIST_URL="https://raw.githubusercontent.com/pexcn/ublacklist/master/ublacklist.txt"

fetch_data() {
  cd $TMP_DIR

  curl -sSL -4 --connect-timeout 10 $UBLACKLIST_URL -o ublacklist.txt

  cd $CUR_DIR
}

gen_ublacklist() {
  cd $TMP_DIR

  sed -i ublacklist.txt \
    -e '/^$/d' \
    -e '/^#/ d' \
    -e 's/^/*:\/\/*./' -e 's/$/\/*/'

  cd $CUR_DIR
}

dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

clean_up() {
  rm -r $TMP_DIR
  echo "[ublacklist]: OK."
}

fetch_data
gen_ublacklist
dist_release
clean_up
