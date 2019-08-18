#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/gfwlist.XXXXXX)

DIST_FILE="dist/gfwlist/gfwlist.txt"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

GFWLIST_URL="https://github.com/gfwlist/gfwlist/raw/master/gfwlist.txt"

function fetch_data() {
  cd $TMP_DIR

  curl -sSL -4 --connect-timeout 10 $GFWLIST_URL | base64 -d > gfwlist.raw
  cp $CUR_DIR/template/gfwlist/gfwlist-extras.txt .
  cp $CUR_DIR/dist/toplist/toplist.txt .

  cd $CUR_DIR
}

function gen_gfw_domain_list() {
  cd $TMP_DIR

  local gfwlist_tmp="gfwlist.tmp"
  local gfwlist_part_1="gfwlist_part_1.tmp"
  local gfwlist_part_2="gfwlist_part_2.tmp"

  # patterns from @cokebar/gfwlist2dnsmasq
  local ignore_pattern='^\!|\[|^@@|(https?://){0,1}[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+'
  local head_filter_pattern='s#^(\|\|?)?(https?://)?##g'
  local tail_filter_pattern='s#/.*$|%2F.*$##g'
  local domain_pattern='([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)'
  local wildcard_pattern='s#^(([a-zA-Z0-9]*\*[-a-zA-Z0-9]*)?(\.))?([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)(\*)?#\4#g'

  # gfwlist filter
  grep -vE $ignore_pattern gfwlist.raw |
    sed -r $head_filter_pattern |
    sed -r $tail_filter_pattern |
    grep -E $domain_pattern |
    sed -r $wildcard_pattern > $gfwlist_tmp
  # append gfwlist extras, sort unique in-place
  sort -u $gfwlist_tmp gfwlist-extras.txt -o $gfwlist_tmp

  # find intersection set
  grep -Fx -f $gfwlist_tmp toplist.txt > $gfwlist_part_1
  # find difference set
  sort $gfwlist_tmp toplist.txt toplist.txt | uniq -u > $gfwlist_part_2
  # sort by toplist
  cat $gfwlist_part_1 $gfwlist_part_2 > $DIST_NAME

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[gfwlist]: OK."
}

fetch_data
gen_gfw_domain_list
dist_release
clean_up
