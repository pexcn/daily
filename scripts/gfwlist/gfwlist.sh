#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/gfwlist.XXXXXX)

SRC_URL_1="https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt"
SRC_URL_2="https://raw.githubusercontent.com/pexcn/gfwlist-extras/master/gfwlist-extras.txt"
SRC_FILE="$CUR_DIR/dist/toplist/toplist.txt"
DEST_FILE="dist/gfwlist/gfwlist.txt"

fetch_src() {
  cd $TMP_DIR

  curl -sSL $SRC_URL_1 | base64 -d > gfwlist-plain.txt
  curl -sSL $SRC_URL_2 -o gfwlist-extras.txt
  cp $SRC_FILE .

  cd $CUR_DIR
}

gen_list() {
  cd $TMP_DIR

  # patterns from @cokebar/gfwlist2dnsmasq#3b5e3560ede7d1b0a1d02157576822752c48e671
  local ignore_pattern='^\!|\[|^@@|(https?://){0,1}[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+'
  local head_filter_pattern='s#^(\|\|?)?(https?://)?##g'
  local tail_filter_pattern='s#/.*$|%2F.*$##g'
  local domain_pattern='([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)'
  local wildcard_pattern='s#^(([a-zA-Z0-9]*\*[-a-zA-Z0-9]*)?(\.))?([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)(\*[a-zA-Z0-9]*)?#\4#g'

  # gfwlist filter
  grep -vE $ignore_pattern gfwlist-plain.txt |
    sed -r $head_filter_pattern |
    sed -r $tail_filter_pattern |
    grep -E $domain_pattern |
    sed -r $wildcard_pattern > gfwlist-plain.tmp
  # gfwlist-extras filter
  sed -e '/^$/d' -e '/^#/ d' gfwlist-extras.txt > gfwlist-extras.tmp
  # merge and deduplication
  cat gfwlist-extras.tmp gfwlist-plain.tmp | awk '!x[$0]++' > gfwlist.tmp

  # find intersection set
  grep -Fx -f gfwlist.tmp toplist.txt > gfwlist_head.tmp
  # find difference set
  grep -Fxv -f toplist.txt gfwlist.tmp > gfwlist_tail.tmp
  # merge to gfwlist
  cat gfwlist_head.tmp gfwlist_tail.tmp > gfwlist.txt

  cd $CUR_DIR
}

copy_dest() {
  install -D $TMP_DIR/gfwlist.txt $DEST_FILE
}

clean_up() {
  rm -r $TMP_DIR
  echo "[$(basename $0 .sh)]: done."
}

fetch_src
gen_list
copy_dest
clean_up
