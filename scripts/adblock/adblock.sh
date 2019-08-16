#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/adblock.XXXXXX)
DIST_DIR="$CUR_DIR/dist/adblock"
DIST_FILE="adblock.conf"

ADLIST_SRC="$CUR_DIR/dist/adlist/adlist.txt"
ADLIST=$(basename $ADLIST_SRC)

function fetch_data() {
  cd $TMP_DIR

  cp $ADLIST_SRC $ADLIST

  cd $CUR_DIR
}

function gen_adblock_list() {
  cd $TMP_DIR

  local adblock_tmp="adblock.tmp"

  sed -e "s/^/address=\//" -e "s/$/\/127.0.0.1/" $ADLIST > $adblock_tmp
  sed -i "1i#\n# Update: $(date +'%Y-%m-%d %T')\n#\n" $adblock_tmp
  head -5004 $adblock_tmp > $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[adblock]: OK."
}

fetch_data
gen_adblock_list
dist_release
clean_up
