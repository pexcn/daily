#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/adlist.XXXXXX)
DIST_DIR="$CUR_DIR/dist/adlist"
DIST_FILE="adlist.txt"

ADAWAY_URL="https://adaway.org/hosts.txt"
EASYLIST_URL="https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt"
YOYO_URL="https://pgl.yoyo.org/adservers/serverlist.php?hostformat=nohtml&showintro=0&mimetype=plaintext"
DISCONNECT_URL="https://s3.amazonaws.com/lists.disconnect.me/simple_malvertising.txt"
ADAWAY_LIST="adaway.txt"
EASY_LIST="easylist.txt"
YOYO_LIST="yoyo.txt"
DISCONNECT_LIST="disconnect.txt"

TOPLIST_SRC="$CUR_DIR/dist/toplist/toplist.txt"
TOPLIST=$(basename $TOPLIST_SRC)

function fetch_data() {
  cd $TMP_DIR

  curl -sSL $ADAWAY_URL > $ADAWAY_LIST
  curl -sSL $EASYLIST_URL > $EASY_LIST
  curl -sSL -k $YOYO_URL > $YOYO_LIST
  curl -sSL $DISCONNECT_URL > $DISCONNECT_LIST
  cp $TOPLIST_SRC $TOPLIST

  cd $CUR_DIR
}

function gen_adlist() {
  cd $TMP_DIR

  local adaway_content="adaway_content.tmp"
  local easylist_content="easylist_content.tmp"
  local yoyo_content="yoyo_content.tmp"
  local disconnect_content="disconnect_content.tmp"

  local adlist_tmp="adlist.tmp"
  local adlist_part_1="adlist_part_1.tmp"
  local adlist_part_2="adlist_part_2.tmp"

  cat $ADAWAY_LIST | sed $"s/\r$//" | sed '/^$/d' | grep -v "^#"  | grep -v "localhost" | cut -d ' ' -f 2 > $adaway_content
  cat $EASY_LIST | grep ^\|\|[^\*]*\^$ | sed -e "s/||//" -e "s/\^//" > $easylist_content
  cat $YOYO_LIST > $yoyo_content
  cat $DISCONNECT_LIST | sed '1,3d' | sed '/^$/d' | grep -v "#" > $disconnect_content

  cat $adaway_content $easylist_content $yoyo_content $disconnect_content | sort -u > $adlist_tmp

  # sort by toplist
  grep -Fx -f $adlist_tmp $TOPLIST > $adlist_part_1
  sort $adlist_tmp $TOPLIST $TOPLIST | uniq -u > $adlist_part_2
  cat $adlist_part_1 $adlist_part_2 > $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[adlist]: OK."
}

fetch_data
gen_adlist
dist_release
clean_up
