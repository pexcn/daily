#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/pac.XXXXXX`
DIST_DIR='dist/pac'
DIST_FILE='whitelist.pac'

CHINA_LIST_URL='https://raw.githubusercontent.com/pexcn/daily/gh-pages/chinalist/chinalist.txt'
CHINA_LIST='chinalist.tmp'

function fetch_data() {
  local pac_template='template/pac/whitelist.pac'

  cp ${pac_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${CHINA_LIST_URL} > ${CHINA_LIST}
  popd
}

function gen_whitelist_pac() {
  pushd ${TMP_DIR}

  sed -i 's/^/  "/' ${CHINA_LIST}
  sed -i 's/$/": 1,/' ${CHINA_LIST}

  sed -i '$ s/.$//g' ${CHINA_LIST}

  sed -i "s/___CHINA_DOMAINS_PLACEHOLDER___/cat ${CHINA_LIST}/e" ${DIST_FILE}
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
