#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/shadowrocket.XXXXXX`
DIST_DIR='dist/shadowrocket'
DIST_FILE='whitelist.conf'

CHINA_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf'
APPLE_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf'
CHNROUTE_URL='https://pexcn.me/daily/chnroute/chnroute.txt'
CHINA_DOMAINS='china_domains.txt'
APPLE_DOMAINS='apple_domains.txt'
CHNROUTE='chnroute.txt'

function fetch_data() {
  local config_template='template/shadowrocket/whitelist.conf'

  cp ${config_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${CHINA_DOMAINS_URL} > ${CHINA_DOMAINS}
  curl -kLs ${APPLE_DOMAINS_URL} > ${APPLE_DOMAINS}
  # todo: copy from dist
  curl -kLs ${CHNROUTE_URL} > ${CHNROUTE}
  popd
}

function gen_whitelist_config() {
  pushd ${TMP_DIR}

  local whitelist_content='whitelist_content.tmp'

  cat ${CHINA_DOMAINS} ${APPLE_DOMAINS} | sort | uniq > ${whitelist_content}

  sed -i 's/server=\//DOMAIN-SUFFIX,/g' ${whitelist_content}
  sed -i 's/\/114.114.114.114/,DIRECT/g' ${whitelist_content}

  sed -i '/#/d' ${whitelist_content}

  sed -i "s/___WHITELIST_DOMAINS_PLACEHOLDER___/cat ${whitelist_content}/e" ${DIST_FILE}
  sed -i "s/___UPDATE_TIME_PLACEHOLDER___/$(date +'%Y-%m-%d %T')/g" ${DIST_FILE}

  sed -i 's/^/IP-CIDR,/' ${CHNROUTE}
  sed -i 's/$/,DIRECT,no-resolve/' ${CHNROUTE}
  sed -i "s/___WHITELIST_IPS_PLACEHOLDER___/cat ${CHNROUTE}/e" ${DIST_FILE}

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
