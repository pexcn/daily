#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/pac.XXXXXX`
DIST_DIR='dist/pac'
DIST_FILE='gfwlist.pac'

GFW_DOMAIN_LIST_URL='https://raw.githubusercontent.com/pexcn/daily/gh-pages/gfwlist/gfwlist.txt'
GFW_DOMAIN_LIST='gfwlist.txt'

function fetch_data() {
  local pac_template='template/pac/gfwlist.pac'

  cp ${pac_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${GFW_DOMAIN_LIST_URL} > ${GFW_DOMAIN_LIST}
  popd
}

function gen_gfwlist_pac() {
  pushd ${TMP_DIR}

  sed -i 's/^/    "/' ${GFW_DOMAIN_LIST}
  sed -i 's/$/": 1,/' ${GFW_DOMAIN_LIST}
  sed -i '$ s/": 1,/": 1/g' ${GFW_DOMAIN_LIST}

  sed -i "s/___GFWLIST_DOMAINS_PLACEHOLDER___/cat ${GFW_DOMAIN_LIST}/e" ${DIST_FILE}
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
gen_gfwlist_pac
dist_release
clean_up
