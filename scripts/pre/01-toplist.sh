#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/toplist.XXXXXX)

DIST_FILE="dist/toplist/toplist.txt"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

TOPLIST_URL="https://s3.amazonaws.com/alexa-static/top-1m.csv.zip"

function gen_toplist() {
  cd $TMP_DIR

  curl -sSL --connect-timeout 10 $TOPLIST_URL |
    # unzip
    gunzip |
    # extract
    awk -F ',' '{print $2}' > $DIST_NAME

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[toplist]: prepared."
}

gen_toplist
dist_release
clean_up
