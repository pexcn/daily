#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/pac.XXXXXX`
DIST_DIR='dist/pac'
DIST_FILE='whitelist.pac'

CHINA_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf'
APPLE_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf'
WHITELIST_DOMAINS='whitelist_domains.txt'

function fetch_data() {
  local pac_template='template/pac/whitelist.pac'

  cp ${pac_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${CHINA_DOMAINS_URL} ${APPLE_DOMAINS_URL} | sort | uniq > ${WHITELIST_DOMAINS}
  popd
}

function gen_whitelist_pac() {
  pushd ${TMP_DIR}

  # get domains
  sed -i 's/server=\//    "/g' ${WHITELIST_DOMAINS}
  sed -i 's/\/114.114.114.114/": 1,/g' ${WHITELIST_DOMAINS}

  # exclude comment lines
  sed -i '/#/d' ${WHITELIST_DOMAINS}

  # remove last ',' character: https://stackoverflow.com/questions/3576139/sed-remove-string-only-in-the-last-line-of-the-file
  sed -i '$ s/": 1,/": 1/g' ${WHITELIST_DOMAINS}

  sed -i "s/___CHINA_DOMAINS_PLACEHOLDER___/cat ${WHITELIST_DOMAINS}/e" ${DIST_FILE}
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
