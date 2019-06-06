#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/chinalist.XXXXXX)
DIST_DIR="$CUR_DIR/dist/chinalist"
DIST_FILE="chinalist.txt"

CHINA_DOMAINS_URL="https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf"
APPLE_DOMAINS_URL="https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf"

function gen_chinalist() {
  cd $TMP_DIR

  curl -sSL $CHINA_DOMAINS_URL $APPLE_DOMAINS_URL |
      # exclude comments
      sed "/#/d" |
      # extract domains
      awk '{split($0, arr, "/"); print arr[2]}' |
      # exclude TLDs
      grep "\." |
      sort | uniq > $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[chinalist]: OK."
}

gen_chinalist
dist_release
clean_up
