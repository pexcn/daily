#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/safelist.XXXXXX)
DIST_DIR="$CUR_DIR/dist/safelist"
DIST_FILE="safelist.conf"

TABOO_LIST_SRC="$CUR_DIR/dist/gfwlist/gfwlist.txt"
TABOO_LIST="taboolist.txt"

function fetch_data() {
  cd $TMP_DIR

  cp $TABOO_LIST_SRC $TABOO_LIST

  cd $CUR_DIR
}

function gen_safe_list() {
  cd $TMP_DIR

  sed -e "s/^/server=\//" -e "s/$/\/127.0.0.1#5300/" $TABOO_LIST > $DIST_FILE
  sed -i "1i#\n# Update: $(date +'%Y-%m-%d %T')\n#\n" $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[safelist]: OK."
}

fetch_data
gen_safe_list
dist_release
clean_up
