#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/adlist.XXXXXX)

DIST_FILE="dist/adlist/adlist.txt"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

ADAWAY_URL="https://adaway.org/hosts.txt"
EASYLIST_URL="https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt"
YOYO_URL="https://pgl.yoyo.org/adservers/serverlist.php?hostformat=nohtml&showintro=0&mimetype=plaintext"
DISCONNECT_URL="https://s3.amazonaws.com/lists.disconnect.me/simple_malvertising.txt"

function fetch_data() {
  cd $TMP_DIR

  curl -sSL -k --connect-timeout 10 $ADAWAY_URL -o adaway.txt
  curl -sSL -k --connect-timeout 10 $EASYLIST_URL -o easylist.txt
  curl -sSL -k --connect-timeout 10 $YOYO_URL -o yoyo.txt
  curl -sSL -k --connect-timeout 10 $DISCONNECT_URL -o disconnect.txt
  cp $CUR_DIR/dist/toplist/toplist.txt .

  cd $CUR_DIR
}

function gen_adlist() {
  cd $TMP_DIR

  local adaway_tmp="adaway.tmp"
  local easylist_tmp="easylist.tmp"
  local yoyo_tmp="yoyo.tmp"
  local disconnect_tmp="disconnect.tmp"

  local adlist_tmp="adlist.tmp"
  local adlist_part_1="adlist_part_1.tmp"
  local adlist_part_2="adlist_part_2.tmp"

  # adaway
  cat adaway.txt |
    # convert to unix format
    sed $'s/\r$//' |
    # ignore empty lines
    sed '/^$/d' |
    # ignore comment lines
    grep -v "^#" |
    # ignore `localhost`
    grep -v "localhost" |
    cut -d ' ' -f 2 > $adaway_tmp

  # easylist
  cat easylist.txt |
    # extract domains
    grep ^\|\|[^\*]*\^$ |
    # clean domains
    sed -e 's/||//' -e 's/\^//' > $easylist_tmp

  # yoyo
  cat yoyo.txt > $yoyo_tmp

  # disconnect
  cat disconnect.txt |
    # ignore 1-3 lines
    sed '1,3d' |
    # ignore empty lines
    sed '/^$/d' |
    # ignore comment lines
    grep -v "#" > $disconnect_tmp

  # merge all list and sort unique
  cat $adaway_tmp $easylist_tmp $yoyo_tmp $disconnect_tmp | sort -u > $adlist_tmp

  # find intersection set
  grep -Fx -f $adlist_tmp toplist.txt > $adlist_part_1
  # find difference set
  sort $adlist_tmp toplist.txt toplist.txt | uniq -u > $adlist_part_2
  # sort by toplist
  cat $adlist_part_1 $adlist_part_2 > $DIST_NAME

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[adlist]: OK."
}

fetch_data
gen_adlist
dist_release
clean_up
