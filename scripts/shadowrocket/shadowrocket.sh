#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/shadowrocket.XXXXXX)

SRC_FILE_1="$CUR_DIR/template/shadowrocket/gfwlist.template"
SRC_FILE_2="$CUR_DIR/template/shadowrocket/whitelist.template"
SRC_FILE_3="$CUR_DIR/dist/gfwlist/gfwlist.txt"
SRC_FILE_4="$CUR_DIR/dist/chinalist/chinalist.txt"
DEST_FILE_1="dist/shadowrocket/gfwlist.conf"
DEST_FILE_2="dist/shadowrocket/whitelist.conf"

fetch_src() {
  cd $TMP_DIR

  cp $SRC_FILE_1 .
  cp $SRC_FILE_2 .
  cp $SRC_FILE_3 .
  cp $SRC_FILE_4 .

  cd $CUR_DIR
}

gen_list() {
  cd $TMP_DIR

  #
  # gfwlist
  #
  # convert to rules content
  sed -e 's/^/DOMAIN-SUFFIX,/' -e 's/$/,PROXY,force-remote-dns/' gfwlist.txt > gfwlist.tmp
  # replace from rules content
  sed 's/___GFWLIST_PLACEHOLDER___/cat gfwlist.tmp/e' gfwlist.template > gfwlist.conf

  # append date tag
  sed -i '1i#' gfwlist.conf
  sed -i "2i# Update: $(date +'%Y-%m-%d %T')" gfwlist.conf
  sed -i '3i#' gfwlist.conf
  sed -i '4i\\' gfwlist.conf

  #
  # whitelist
  #
  # convert to rules content, limit 2000 entries
  head -2000 chinalist.txt | sed -e 's/^/DOMAIN-SUFFIX,/' -e 's/$/,DIRECT/' > chinalist.tmp
  # replace from rules content
  sed 's/___CHINALIST_PLACEHOLDER___/cat chinalist.tmp/e' whitelist.template > whitelist.conf

  # append date tag
  sed -i '1i#' whitelist.conf
  sed -i "2i# Update: $(date +'%Y-%m-%d %T')" whitelist.conf
  sed -i '3i#' whitelist.conf
  sed -i '4i\\' whitelist.conf

  cd $CUR_DIR
}

copy_dest() {
  install -D $TMP_DIR/gfwlist.conf $DEST_FILE_1
  install -D $TMP_DIR/whitelist.conf $DEST_FILE_2
}

clean_up() {
  rm -r $TMP_DIR
  echo "[$(basename $0 .sh)]: done."
}

fetch_src
gen_list
copy_dest
clean_up
