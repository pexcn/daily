#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/root-domain.XXXXXX)

SRC_FILE_1="$CUR_DIR/dist/gfwlist/gfwlist-root.txt"
SRC_FILE_2="$CUR_DIR/dist/chinalist/chinalist-root.txt"
DEST_FILE_1="dist/gfwlist/gfwlist-root.txt"
DEST_FILE_2="dist/chinalist/chinalist-root.txt"

fetch_src() {
  cd $TMP_DIR

  cp $SRC_FILE_1 .
  cp $SRC_FILE_2 .

  cd $CUR_DIR
}

gen_list() {
  cd $TMP_DIR

  # find intersection set
  grep -Fx -f gfwlist-root.txt chinalist-root.txt > common.tmp
  # gfwlist-root purify
  grep -Fxv -f common.tmp gfwlist-root.txt > gfwlist-root.new
  # chinalist-root purify
  grep -Fxv -f common.tmp chinalist-root.txt > chinalist-root.new

  cd $CUR_DIR
}

copy_dest() {
  install -D -m 644 $TMP_DIR/gfwlist-root.new $DEST_FILE_1
  install -D -m 644 $TMP_DIR/chinalist-root.new $DEST_FILE_2
}

clean_up() {
  rm -r $TMP_DIR
}

fetch_src
gen_list
copy_dest
clean_up
