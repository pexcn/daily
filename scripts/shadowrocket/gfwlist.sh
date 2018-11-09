#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/shadowrocket.XXXXXX`
DIST_DIR='dist/shadowrocket'
DIST_FILE='gfwlist.conf'

GFW_DOMAIN_LIST_URL='https://raw.githubusercontent.com/pexcn/daily/gh-pages/gfwlist/gfwlist.txt'
GFW_DOMAIN_LIST='gfwlist.txt'

function fetch_data() {
  local config_template='template/shadowrocket/gfwlist.conf'

  cp ${config_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${GFW_DOMAIN_LIST_URL} > ${GFW_DOMAIN_LIST}
  popd
}

function gen_gfwlist_config() {
  pushd ${TMP_DIR}

  sed -i 's/^/DOMAIN-SUFFIX,/' ${GFW_DOMAIN_LIST}
  sed -i 's/$/,PROXY,force-remote-dns/' ${GFW_DOMAIN_LIST}

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
gen_gfwlist_config
dist_release
clean_up
