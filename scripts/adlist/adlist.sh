#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/adlist.XXXXXX)
DIST_DIR="$CUR_DIR/dist/adlist"
DIST_FILE="adlist.txt"
DIST_FILE_FULL="adlist-full.txt"

EASYLIST_URL="https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt"
YOYO_URL="https://pgl.yoyo.org/adservers/serverlist.php?hostformat=hosts&showintro=0&mimetype=plaintext"
ADAWAY_URL="https://hosts-file.net/ad_servers.txt"

EASY_LIST="easylistchina.txt"
YOYO_LIST="yoyo.txt"
ADAWAY_LIST="adaway.txt"

TOPLIST_SRC="$CUR_DIR/dist/toplist/toplist.txt"
TOPLIST=$(basename $TOPLIST_SRC)

function fetch_data() {
  cd $TMP_DIR

  curl -sSL $EASYLIST_URL > $EASY_LIST
  curl -ksSL $YOYO_URL > $YOYO_LIST
  curl -sSL $ADAWAY_URL > $ADAWAY_LIST
  cp $TOPLIST_SRC $TOPLIST

  cd $CUR_DIR
}

function gen_adlist() {
  cd $TMP_DIR

  local easylist_content="easylistchina_content.tmp"
  local yoyo_content="yoyo_content.tmp"
  local adaway_content="adaway_content.tmp"

  local adlist_tmp="adlist.tmp"
  local adlist_part_1="adlist_part_1.tmp"
  local adlist_part_2="adlist_part_2.tmp"
  local adlist_full_tmp="adlist-full.tmp"
  local adlist_full_part_1="adlist-full_part_1.tmp"
  local adlist_full_part_2="adlist-full_part_2.tmp"

  cat $EASY_LIST | grep ^\|\|[^\*]*\^$ | sed -e "s/||//" -e "s/\^//" > $easylist_content
  cat $YOYO_LIST | grep -E "^127.0.0.1" | sed "s/127.0.0.1 //" > $yoyo_content
  cat $ADAWAY_LIST | sed $"s/\r$//" | grep -E "^127.0.0.1" | grep -v "#" | sed "s/127.0.0.1\t//" > $adaway_content

  cat $easylist_content $yoyo_content | sort -u > $adlist_tmp
  cat $easylist_content $yoyo_content $adaway_content | sort -u > $adlist_full_tmp

  # sort by toplist
  grep -Fx -f $adlist_tmp $TOPLIST > $adlist_part_1
  sort $adlist_tmp $TOPLIST $TOPLIST | uniq -u > $adlist_part_2
  cat $adlist_part_1 $adlist_part_2 > $DIST_FILE

  grep -Fx -f $adlist_full_tmp $TOPLIST > $adlist_full_part_1
  sort $adlist_full_tmp $TOPLIST $TOPLIST | uniq -u > $adlist_full_part_2
  cat $adlist_full_part_1 $adlist_full_part_2 > $DIST_FILE_FULL

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
