#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/chinalist.XXXXXX`
DIST_DIR='dist/chinalist'
DIST_FILE='chinalist.txt'

CHINA_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf'
APPLE_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf'

function gen_chinalist() {
  pushd ${TMP_DIR}

  curl -kLs ${CHINA_DOMAINS_URL} ${APPLE_DOMAINS_URL} |
      # exclude comments
      sed '/#/d' |
      # extract domains
      awk '{split($0, arr, "/"); print arr[2]}' |
      # exclude TLDs
      grep '\.' |
      sort | uniq > ${DIST_FILE}

  popd
}

function dist_release() {
  mkdir -p ${DIST_DIR}
  mv ${TMP_DIR}/${DIST_FILE} ${DIST_DIR}
}

function clean_up() {
  rm -r ${TMP_DIR}
}

gen_chinalist
dist_release
clean_up
