#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/pac.XXXXXX)
DIST_DIR="$CUR_DIR/dist/pac"
DIST_FILE="gfwlist.pac"

GFW_DOMAIN_LIST_SRC="$CUR_DIR/dist/gfwlist/gfwlist.txt"
GFW_DOMAIN_LIST=$(basename $GFW_DOMAIN_LIST_SRC)

function fetch_data() {
  cd $TMP_DIR

  local pac_template="$CUR_DIR/template/pac/gfwlist.pac"

  cp $pac_template .
  cp $GFW_DOMAIN_LIST_SRC $GFW_DOMAIN_LIST 

  cd $CUR_DIR
}

function gen_gfwlist_pac() {
  cd $TMP_DIR

  sed -i 's/^/  "/' $GFW_DOMAIN_LIST
  sed -i 's/$/": 1,/' $GFW_DOMAIN_LIST
  sed -i '$ s/": 1,/": 1/g' $GFW_DOMAIN_LIST

  sed -i "s/___GFWLIST_DOMAINS_PLACEHOLDER___/cat $GFW_DOMAIN_LIST/e" $DIST_FILE
  sed -i "s/___UPDATE_TIME_PLACEHOLDER___/$(date +'%Y-%m-%d %T')/g" $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
}

fetch_data
gen_gfwlist_pac
dist_release
clean_up
