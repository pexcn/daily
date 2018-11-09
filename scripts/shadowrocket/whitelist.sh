#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/shadowrocket.XXXXXX`
DIST_DIR='dist/shadowrocket'
DIST_FILE='whitelist.conf'

CHINA_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf'
APPLE_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf'
TOP_5000_URL='https://pexcn.me/AlexaTopSites/top-5000.txt'

CHINA_LIST='china-list.txt'
TOP_LIST='top-list.txt'
WHITELIST_CONTENT='whitelist_content.tmp'

function fetch_data() {
  local config_template='template/shadowrocket/whitelist.conf'

  cp ${config_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  # exclude comments & extract domains
  curl -kLs ${CHINA_DOMAINS_URL} ${APPLE_DOMAINS_URL} |
      sed '/#/d' |
      awk '{split($0, arr, "/"); print arr[2]}' |
      grep '\.' |
      sort | uniq > ${CHINA_LIST}

  curl -kLs ${TOP_5000_URL} > ${TOP_LIST}
  popd
}

function gen_whitelist_config() {
  pushd ${TMP_DIR}

  grep -Fx -f ${CHINA_LIST} ${TOP_LIST} > ${WHITELIST_CONTENT}

  sed -i 's/^/DOMAIN-SUFFIX,/' ${WHITELIST_CONTENT}
  sed -i 's/$/,DIRECT/' ${WHITELIST_CONTENT}

  sed -i "s/___WHITELIST_DOMAINS_PLACEHOLDER___/cat ${WHITELIST_CONTENT}/e" ${DIST_FILE}
  sed -i "s/___UPDATE_TIME_PLACEHOLDER___/$(date +'%Y-%m-%d %T')/g" ${DIST_FILE}

  popd
}

function dist_release() {
  mkdir -p ${DIST_DIR}
  mv ${TMP_DIR}/${DIST_FILE} ${DIST_DIR}
}

function clean_up() {
  rm -r ${TMP_DIR}
}

fetch_data
gen_whitelist_config
dist_release
clean_up
