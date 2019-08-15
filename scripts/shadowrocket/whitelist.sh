#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/shadowrocket.XXXXXX)
DIST_DIR="$CUR_DIR/dist/shadowrocket"
DIST_FILE="whitelist.conf"

CHINA_LIST_SRC="$CUR_DIR/dist/chinalist/chinalist.txt"
CHINA_LIST=$(basename $CHINA_LIST_SRC)
TOP_LIST_SRC="$CUR_DIR/dist/alexa/top-cn.txt"
TOP_LIST=$(basename $TOP_LIST_SRC)

function fetch_data() {
  cd $TMP_DIR

  local config_template="$CUR_DIR/template/shadowrocket/whitelist.conf"

  cp $config_template .
  cp $CHINA_LIST_SRC $CHINA_LIST
  cp $TOP_LIST_SRC $TOP_LIST

  cd $CUR_DIR
}

function gen_whitelist_config() {
  cd $TMP_DIR

  local tmplist="whitelist.tmp"

  grep -Fx -f $CHINA_LIST $TOP_LIST | head -2000 > $tmplist
  sed -i "s/^/DOMAIN-SUFFIX,/" $tmplist
  sed -i "s/$/,DIRECT/" $tmplist
  sed -i "s/___WHITELIST_DOMAINS_PLACEHOLDER___/cat $tmplist/e" $DIST_FILE
  sed -i "s/___UPDATE_TIME_PLACEHOLDER___/$(date +'%Y-%m-%d %T')/g" $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[shadowrocket -> whitelist]: OK."
}

fetch_data
gen_whitelist_config
dist_release
clean_up
