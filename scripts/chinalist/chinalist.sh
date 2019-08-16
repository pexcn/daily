#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/chinalist.XXXXXX)
DIST_DIR="$CUR_DIR/dist/chinalist"
DIST_FILE="chinalist.txt"

CHINA_DOMAINS_URL="https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf"
APPLE_DOMAINS_URL="https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf"
TOPLIST_SRC="$CUR_DIR/dist/toplist/toplist.txt"
TOPLIST=$(basename $TOPLIST_SRC)

function fetch_data() {
  cd $TMP_DIR

  cp $TOPLIST_SRC $TOPLIST

  cd $CUR_DIR
}

function gen_chinalist() {
  cd $TMP_DIR

  local chinalist_tmp="chinalist.tmp"
  local chinalist_part_1="chinalist_part_1"
  local chinalist_part_2="chinalist_part_2"

  curl -sSL $CHINA_DOMAINS_URL $APPLE_DOMAINS_URL |
      # exclude comments
      sed "/#/d" |
      # extract domains
      awk '{split($0, arr, "/"); print arr[2]}' |
      # exclude TLDs
      grep "\." |
      sort -u > $chinalist_tmp

  # sort by toplist
  grep -Fx -f $chinalist_tmp $TOPLIST > $chinalist_part_1
  sort $chinalist_tmp $TOPLIST $TOPLIST | uniq -u > $chinalist_part_2

  cat $chinalist_part_1 $chinalist_part_2 > $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[chinalist]: OK."
}

fetch_data
gen_chinalist
dist_release
clean_up
