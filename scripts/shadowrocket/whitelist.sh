#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/shadowrocket.XXXXXX`
DIST_DIR='dist/shadowrocket'
DIST_FILE='whitelist.conf'

CHINA_LIST_URL='https://raw.githubusercontent.com/pexcn/daily/gh-pages/chinalist/chinalist.txt'
CHINA_LIST='chinalist.txt'
TOP_LIST_URL='https://raw.githubusercontent.com/pexcn/AlexaTopSites/gh-pages/top-5000.txt'
TOP_LIST='toplist.txt'

WHITELIST='whitelist.tmp'

function fetch_data() {
  local config_template='template/shadowrocket/whitelist.conf'

  cp ${config_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${CHINA_LIST_URL} > ${CHINA_LIST}
  curl -kLs ${TOP_LIST_URL} > ${TOP_LIST}
  popd
}

function gen_whitelist_config() {
  pushd ${TMP_DIR}

  grep -Fx -f ${CHINA_LIST} ${TOP_LIST} > ${WHITELIST}

  sed -i 's/^/DOMAIN-SUFFIX,/' ${WHITELIST}
  sed -i 's/$/,DIRECT/' ${WHITELIST}

  sed -i "s/___WHITELIST_DOMAINS_PLACEHOLDER___/cat ${WHITELIST}/e" ${DIST_FILE}
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
