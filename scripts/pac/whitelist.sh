#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/pac.XXXXXX`
DIST_DIR='dist/pac'
DIST_FILE='whitelist.pac'

PAC_TEMPLATE='template/pac/whitelist.pac'

CHINA_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf'
APPLE_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf'

CHINA_DOMAINS='china_domains.txt'
APPLE_DOMAINS='apple_domains.txt'

function fetch_data() {
  cp ${PAC_TEMPLATE} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${CHINA_DOMAINS_URL} > ${CHINA_DOMAINS}
  curl -kLs ${APPLE_DOMAINS_URL} > ${APPLE_DOMAINS}
  popd
}

function gen_whitelist_pac() {
  pushd ${TMP_DIR}

  local whitelist_content='whitelist_content.tmp'

  # merge whitelist
  cat ${CHINA_DOMAINS} ${APPLE_DOMAINS} > ${whitelist_content}

  # get domains
  sed -i 's/server=\//    "/g' ${whitelist_content}
  sed -i 's/\/114.114.114.114/": 1,/g' ${whitelist_content}

  # exclude comment lines
  sed -i '/#/d' ${whitelist_content}

  # remove last ',' character: https://stackoverflow.com/questions/3576139/sed-remove-string-only-in-the-last-line-of-the-file
  sed -i '$ s/": 1,/": 1/g' ${whitelist_content}

  sed -i "s/___CHINA_DOMAINS_PLACEHOLDER___/cat ${whitelist_content}/e" ${DIST_FILE}

  sed -i "1i//\n// Update: $(date +'%Y-%m-%d %T')\n//\n" ${DIST_FILE}
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
#clean_up
