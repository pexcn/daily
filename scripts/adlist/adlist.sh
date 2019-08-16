#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/adlist.XXXXXX)
DIST_DIR="$CUR_DIR/dist/adlist"
DIST_FILE="adlist.txt"
DIST_FILE_FULL="adlist-full.txt"

EASYLIST_URL="https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt"
ABPFX_URL="https://raw.githubusercontent.com/xinggsf/Adblock-Plus-Rule/master/ABP-FX.txt"
YOYO_URL="https://pgl.yoyo.org/adservers/serverlist.php?hostformat=hosts&showintro=0&mimetype=plaintext"
ADAWAY_URL="https://hosts-file.net/ad_servers.txt"

EASY_LIST="easylistchina.txt"
ABPFX_LIST="abpfx.txt"
YOYO_LIST="yoyo.txt"
ADAWAY_LIST="adaway.txt"

function fetch_data() {
  cd $TMP_DIR

  curl -sSL $EASYLIST_URL > $EASY_LIST
  curl -sSL $ABPFX_URL > $ABPFX_LIST
  curl -sSL $YOYO_URL > $YOYO_LIST
  curl -sSL $ADAWAY_URL > $ADAWAY_LIST

  cd $CUR_DIR
}

function gen_adlist() {
  cd $TMP_DIR

  local easylist_content="easylistchina_content.tmp"
  local abpfx_content="abpfx_content.tmp"
  local yoyo_content="yoyo_content.tmp"
  local adaway_content="adaway_content.tmp"

  cat $EASY_LIST | grep ^\|\|[^\*]*\^$ | sed -e "s/||//" -e "s/\^//" > $easylist_content
  cat $ABPFX_LIST | grep ^\|\|[^\*]*\^$ | sed -e "s/||//" -e "s/\^//" > $abpfx_content
  cat $YOYO_LIST | grep -E "^127.0.0.1" | sed "s/127.0.0.1 //" > $yoyo_content
  cat $ADAWAY_LIST | sed $"s/\r$//" | grep -E "^127.0.0.1" | grep -v "#" | sed "s/127.0.0.1\t//" > $adaway_content

  cat $easylist_content $abpfx_content $yoyo_content | sort -u > $DIST_FILE
  cat $easylist_content $abpfx_content $yoyo_content $adaway_content | sort -u > $DIST_FILE_FULL

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_FULL $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[adlist]: OK."
}

fetch_data
gen_adlist
dist_release
clean_up
