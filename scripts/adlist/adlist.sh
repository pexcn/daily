#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/adlist.XXXXXX)

SRC_URL_1="https://adguardteam.github.io/AdGuardSDNSFilter/Filters/filter.txt"
SRC_URL_2="https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt"
SRC_URL_3="https://pgl.yoyo.org/adservers/serverlist.php?hostformat=nohtml&showintro=0&mimetype=plaintext"
SRC_FILE="$CUR_DIR/dist/toplist/toplist.txt"
DEST_FILE="dist/adlist/adlist.txt"

fetch_src() {
  cd $TMP_DIR

  curl -sSLk4 $SRC_URL_1 -o adguard.txt
  curl -sSLk4 $SRC_URL_2 -o easylist.txt
  curl -sSLk4 $SRC_URL_3 -o yoyo.txt
  cp $SRC_FILE .

  cd $CUR_DIR
}

gen_list() {
  cd $TMP_DIR

  local ipv4_regex="[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}"

  #
  # adguard
  #
  cat adguard.txt |
    # add newline to end of file only if newline doesn't exist
    sed '$a\' |
    # extract domain
    grep -E '^\|\|[^\*]*\^$' |
    sed -e 's/||//' -e 's/\^//' |
    # remove ipv4 address
    grep -v "$ipv4_regex" |
    # remove duplicates
    awk '!x[$0]++' > adguard.tmp

  #
  # easylist
  #
  cat easylist.txt |
    # add newline to end of file only if newline doesn't exist
    sed '$a\' |
    # extract domain
    grep -E '^\|\|[^\*]*\^$' |
    sed -e 's/||//' -e 's/\^//' |
    # remove ipv4 address
    grep -v "$ipv4_regex" |
    # remove duplicates
    awk '!x[$0]++' > easylist.tmp

  #
  # yoyo
  #
  cat yoyo.txt > yoyo.tmp

  # merge all list and remove duplicates
  cat adguard.tmp easylist.tmp yoyo.tmp | awk '!x[$0]++' > adlist.tmp

  #
  # sorting by toplist
  #
  # find intersection set
  grep -Fx -f adlist.tmp toplist.txt > adlist_head.tmp
  # find difference set
  grep -Fxv -f toplist.txt adlist.tmp > adlist_tail.tmp
  # merge to adlist
  cat adlist_head.tmp adlist_tail.tmp > adlist.txt

  cd $CUR_DIR
}

copy_dest() {
  install -D -m 644 $TMP_DIR/adlist.txt $DEST_FILE
}

clean_up() {
  rm -r $TMP_DIR
  echo "[$(basename $0 .sh)]: done."
}

fetch_src
gen_list
copy_dest
clean_up
