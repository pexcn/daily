#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/pac.XXXXXX)
DIST_DIR="$CUR_DIR/dist/pac"
DIST_FILE="whitelist.pac"

CHINA_LIST_SRC="$CUR_DIR/dist/chinalist/chinalist.txt"
CHINA_LIST=$(basename $CHINA_LIST_SRC)

function fetch_data() {
  cd $TMP_DIR

  local pac_template="$CUR_DIR/template/pac/whitelist.pac"

  cp $pac_template .
  cp $CHINA_LIST_SRC $CHINA_LIST

  cd $CUR_DIR
}

function gen_whitelist_pac() {
  cd $TMP_DIR

  sed -i 's/^/  "/' $CHINA_LIST
  sed -i 's/$/": 1,/' $CHINA_LIST

  sed -i '$ s/.$//g' $CHINA_LIST

  sed -i "s/___CHINA_DOMAINS_PLACEHOLDER___/cat $CHINA_LIST/e" $DIST_FILE
  sed -i "s/___UPDATE_TIME_PLACEHOLDER___/$(date +'%Y-%m-%d %T')/g" $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[pac/whitelist]: OK."
}

fetch_data
gen_whitelist_pac
dist_release
clean_up
