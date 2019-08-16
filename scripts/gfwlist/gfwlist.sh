#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/gfwlist.XXXXXX)
DIST_DIR="$CUR_DIR/dist/gfwlist"
DIST_FILE="gfwlist.txt"

GFWLIST_URL="https://github.com/gfwlist/gfwlist/raw/master/gfwlist.txt"
GFWLIST="gfwlist-origin.txt"
TOPLIST_SRC="$CUR_DIR/dist/toplist/toplist.txt"
TOPLIST=$(basename $TOPLIST_SRC)

function fetch_data() {
  cd $TMP_DIR

  local gfwlist_extras_template="$CUR_DIR/template/gfwlist/gfwlist-extras.txt"

  cp $gfwlist_extras_template .
  curl -sSL $GFWLIST_URL | base64 -d > $GFWLIST
  cp $TOPLIST_SRC $TOPLIST

  cd $CUR_DIR
}

function gen_gfw_domain_list() {
  cd $TMP_DIR

  local gfwlist_tmp="gfwlist.tmp"
  local gfwlist_extras="gfwlist-extras.txt"
  local gfwlist_part_1="gfwlist_part_1.tmp"
  local gfwlist_part_2="gfwlist_part_2.tmp"

  # patterns from @cokebar/gfwlist2dnsmasq
  local ignore_pattern='^\!|\[|^@@|(https?://){0,1}[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+'
  local head_filter_pattern='s#^(\|\|?)?(https?://)?##g'
  local tail_filter_pattern='s#/.*$|%2F.*$##g'
  local domain_pattern='([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)'
  local wildcard_pattern='s#^(([a-zA-Z0-9]*\*[-a-zA-Z0-9]*)?(\.))?([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)(\*)?#\4#g'

  grep -vE $ignore_pattern $GFWLIST |
      sed -r $head_filter_pattern |
      sed -r $tail_filter_pattern |
      grep -E $domain_pattern |
      sed -r $wildcard_pattern > $gfwlist_tmp
  cat $gfwlist_extras >> $gfwlist_tmp
  sort -u -o $gfwlist_tmp $gfwlist_tmp

  # sort by toplist
  grep -Fx -f $gfwlist_tmp $TOPLIST > $gfwlist_part_1
  sort $gfwlist_tmp $TOPLIST $TOPLIST | uniq -u > $gfwlist_part_2

  cat $gfwlist_part_1 $gfwlist_part_2 > $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[gfwlist]: OK."
}

fetch_data
gen_gfw_domain_list
dist_release
clean_up
