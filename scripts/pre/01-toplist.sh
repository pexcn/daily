#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/toplist.XXXXXX)
DIST_DIR="$CUR_DIR/dist/toplist"
DIST_FILE="toplist.txt"

TOPLIST_URL="https://s3.amazonaws.com/alexa-static/top-1m.csv.zip"

function gen_toplist() {
  cd $TMP_DIR

  curl -sSL $TOPLIST_URL | gunzip | awk -F ',' '{print $2}' > $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[toplist]: prepared."
}

gen_toplist
dist_release
clean_up
