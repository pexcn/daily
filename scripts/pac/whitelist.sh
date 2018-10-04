#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/pac.XXXXXX`
DIST_DIR='dist/pac'
DIST_FILE='whitelist.pac'

CHINA_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf'
APPLE_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf'
WHITELIST_CONTENT='whitelist_content.tmp'

function fetch_data() {
  local pac_template='template/pac/whitelist.pac'

  cp ${pac_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  # exclude comments & extract domains
  curl -kLs ${CHINA_DOMAINS_URL} ${APPLE_DOMAINS_URL} |
      sed '/#/d' | awk '{split($0, arr, "/"); print arr[2]}' |
      grep '\.' |
      sort | uniq > ${WHITELIST_CONTENT}
  popd
}

function gen_whitelist_pac() {
  pushd ${TMP_DIR}

  # replace content
  sed -i 's/^/    "/' ${WHITELIST_CONTENT}
  sed -i 's/$/": 1,/' ${WHITELIST_CONTENT}

  # remove last ',' character: https://stackoverflow.com/questions/3576139/sed-remove-string-only-in-the-last-line-of-the-file
  sed -i '$ s/": 1,/": 1/g' ${WHITELIST_CONTENT}

  sed -i "s/___CHINA_DOMAINS_PLACEHOLDER___/cat ${WHITELIST_CONTENT}/e" ${DIST_FILE}
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
gen_whitelist_pac
dist_release
clean_up
