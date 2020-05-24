#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/pac.XXXXXX)

SRC_FILE_1="$CUR_DIR/template/pac/gfwlist.template"
SRC_FILE_2="$CUR_DIR/template/pac/whitelist.template"
SRC_FILE_3="$CUR_DIR/dist/gfwlist/gfwlist.txt"
SRC_FILE_4="$CUR_DIR/dist/chinalist/chinalist.txt"
DEST_FILE_1="dist/pac/gfwlist.pac"
DEST_FILE_2="dist/pac/whitelist.pac"

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
  # convert to pac content
  sed -e 's/^/  "/' -e 's/$/": 1,/' gfwlist.txt > gfwlist.tmp
  # remove the last ',' character
  sed -i '$ s/.$//' gfwlist.tmp
  # replace from pac content
  sed 's/___GFWLIST_PLACEHOLDER___/cat gfwlist.tmp/e' gfwlist.template > gfwlist.pac

  # append date tag
  sed -i '1i//' gfwlist.pac
  sed -i "2i// Update: $(date +'%Y-%m-%d %T')" gfwlist.pac
  sed -i '3i//' gfwlist.pac
  sed -i '4i\\' gfwlist.pac

  #
  # whitelist
  #
  # convert to pac content
  sed -e 's/^/  "/' -e 's/$/": 1,/' chinalist.txt > chinalist.tmp
  # remove the last ',' character
  sed -i '$ s/.$//' chinalist.tmp
  # replace from pac content
  sed 's/___CHINALIST_PLACEHOLDER___/cat chinalist.tmp/e' whitelist.template > whitelist.pac

  # append date tag
  sed -i '1i//' whitelist.pac
  sed -i "2i// Update: $(date +'%Y-%m-%d %T')" whitelist.pac
  sed -i '3i//' whitelist.pac
  sed -i '4i\\' whitelist.pac

  cd $CUR_DIR
}

copy_dest() {
  install -D $TMP_DIR/gfwlist.pac $DEST_FILE_1
  install -D $TMP_DIR/whitelist.pac $DEST_FILE_2
}

clean_up() {
  rm -r $TMP_DIR
  echo "[$(basename $0 .sh)]: done."
}

fetch_src
gen_list
copy_dest
clean_up
