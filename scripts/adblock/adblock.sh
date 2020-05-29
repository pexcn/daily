#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/adblock.XXXXXX)

SRC_FILE="$CUR_DIR/dist/adlist/adlist.txt"
DEST_FILE="dist/adblock/adblock.conf"

fetch_src() {
  cd $TMP_DIR

  cp $SRC_FILE .

  cd $CUR_DIR
}

gen_list() {
  cd $TMP_DIR

  local ipv4_regex="[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}"

  # ignore ipv4 address
  grep -v "$ipv4_regex" adlist.txt > adblock.conf
  # convert to dnsmasq format
  sed -i -e 's/^/server=\//' -e 's/$/\//' adblock.conf

  # append date tag
  sed -i '1i#' adblock.conf
  sed -i "2i# Update: $(date +'%Y-%m-%d %T')" adblock.conf
  sed -i '3i#' adblock.conf
  sed -i '4i\\' adblock.conf

  cd $CUR_DIR
}

copy_dest() {
  install -D -m 644 $TMP_DIR/adblock.conf $DEST_FILE
}

clean_up() {
  rm -r $TMP_DIR
  echo "[$(basename $0 .sh)]: done."
}

fetch_src
gen_list
copy_dest
clean_up
