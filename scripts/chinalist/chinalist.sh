#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/chinalist.XXXXXX)

DIST_FILE="dist/chinalist/chinalist.txt"
DIST_FILE_LITE="dist/chinalist/chinalist-lite.txt"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"
DIST_NAME_LITE="$(basename $DIST_FILE_LITE)"

CHINA_DOMAIN_URL="https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf"
APPLE_DOMAIN_URL="https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf"
GOOGLE_DOMAIN_URL="https://github.com/felixonmars/dnsmasq-china-list/raw/master/google.china.conf"

fetch_data() {
  cd $TMP_DIR

  curl -sSL -4 --connect-timeout 10 $CHINA_DOMAIN_URL -o china.conf
  curl -sSL -4 --connect-timeout 10 $APPLE_DOMAIN_URL -o apple.conf
  curl -sSL -4 --connect-timeout 10 $GOOGLE_DOMAIN_URL -o google.conf
  cp $CUR_DIR/dist/toplist/toplist.txt .

  cd $CUR_DIR
}

gen_chinalist() {
  cd $TMP_DIR

  local chinalist_tmp="chinalist.tmp"
  local chinalist_part_1="chinalist_part_1.tmp"
  local chinalist_part_2="chinalist_part_2.tmp"

  cat china.conf apple.conf google.conf |
      # ignore comments
      sed "/#/d" |
      # extract domains
      awk '{split($0, arr, "/"); print arr[2]}' |
      # exclude TLDs
      grep "\." |
      # sort unique
      sort -u > $chinalist_tmp

  # find intersection set
  grep -Fx -f $chinalist_tmp toplist.txt > $chinalist_part_1
  # find difference set
  sort $chinalist_tmp toplist.txt toplist.txt | uniq -u > $chinalist_part_2
  # sort by toplist
  cat $chinalist_part_1 $chinalist_part_2 > $DIST_NAME

  # make part 1 as lite version
  cp $chinalist_part_1 $DIST_NAME_LITE

  cd $CUR_DIR
}

dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
  cp $TMP_DIR/$DIST_NAME_LITE $DIST_FILE_LITE
}

clean_up() {
  rm -r $TMP_DIR
  echo "[chinalist]: OK."
}

fetch_data
gen_chinalist
dist_release
clean_up
