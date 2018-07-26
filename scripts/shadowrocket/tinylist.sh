#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/shadowrocket.XXXXXX`
DIST_DIR='dist/shadowrocket'
DIST_FILE='tinylist.conf'

TINYLIST_URL='https://github.com/gfwlist/tinylist/raw/master/tinylist.txt'
TINYLIST='tinylist.txt'

TINYLIST_CONTENT='tinylist_content.tmp'

function fetch_data() {
  local config_template='template/shadowrocket/tinylist.conf'
  local extra_template='template/gfwlist_extra_domains.txt'

  cp ${config_template} ${TMP_DIR}
  cp ${extra_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${TINYLIST_URL} | base64 -d > ${TINYLIST}
  popd
}

function extract_domains() {
  pushd ${TMP_DIR}

  local tinylist_domains='tinylist_domains.txt'
  local gfwlist_extra_domains='gfwlist_extra_domains.txt'

  # patterns from @cokebar/gfwlist2dnsmasq
  local ignore_pattern='^\!|\[|^@@|(https?://){0,1}[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+'
  local head_filter_pattern='s#^(\|\|?)?(https?://)?##g'
  local tail_filter_pattern='s#/.*$|%2F.*$##g'
  local domain_pattern='([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)'
  local wildcard_pattern='s#^(([a-zA-Z0-9]*\*[-a-zA-Z0-9]*)?(\.))?([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)(\*)?#\4#g'

  grep -vE ${ignore_pattern} ${TINYLIST} |
      sed -r ${head_filter_pattern} |
      sed -r ${tail_filter_pattern} |
      grep -E ${domain_pattern} |
      sed -r ${wildcard_pattern} > ${tinylist_domains}
  cat ${gfwlist_extra_domains} >> ${tinylist_domains}

  cat ${tinylist_domains} | sort | uniq > ${TINYLIST_CONTENT}

  popd
}

function gen_tinylist_config() {
  pushd ${TMP_DIR}

  sed -i 's/^/DOMAIN-SUFFIX,/' ${TINYLIST_CONTENT}
  sed -i 's/$/,PROXY,force-remote-dns/' ${TINYLIST_CONTENT}

  sed -i "s/___TINYLIST_DOMAINS_PLACEHOLDER___/cat ${TINYLIST_CONTENT}/e" ${DIST_FILE}

  sed -i "1i#\n# Update: $(date +'%Y-%m-%d %T')\n#\n" ${DIST_FILE}

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
extract_domains
gen_tinylist_config
dist_release
clean_up
