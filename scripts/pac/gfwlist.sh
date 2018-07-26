#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/pac.XXXXXX`
DIST_DIR='dist/pac'
DIST_FILE='gfwlist.pac'

GFWLIST_URL='https://github.com/gfwlist/gfwlist/raw/master/gfwlist.txt'
GFWLIST='gfwlist.txt'

GFWLIST_CONTENT='gfwlist_content.tmp'

function fetch_data() {
  local pac_template='template/pac/gfwlist.pac'
  local extra_template='template/gfwlist_extra_domains.txt'

  cp ${pac_template} ${TMP_DIR}
  cp ${extra_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${GFWLIST_URL} | base64 -d > ${GFWLIST}
  popd
}

function extract_domains() {
  pushd ${TMP_DIR}

  local gfwlist_domains='gfwlist_domains.txt'
  local gfwlist_extra_domains='gfwlist_extra_domains.txt'

  # patterns from @cokebar/gfwlist2dnsmasq
  local ignore_pattern='^\!|\[|^@@|(https?://){0,1}[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+'
  local head_filter_pattern='s#^(\|\|?)?(https?://)?##g'
  local tail_filter_pattern='s#/.*$|%2F.*$##g'
  local domain_pattern='([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)'
  local wildcard_pattern='s#^(([a-zA-Z0-9]*\*[-a-zA-Z0-9]*)?(\.))?([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)(\*)?#\4#g'

  grep -vE ${ignore_pattern} ${GFWLIST} |
      sed -r ${head_filter_pattern} |
      sed -r ${tail_filter_pattern} |
      grep -E ${domain_pattern} |
      sed -r ${wildcard_pattern} > ${gfwlist_domains}
  cat ${gfwlist_extra_domains} >> ${gfwlist_domains}

  cat ${gfwlist_domains} | sort | uniq > ${GFWLIST_CONTENT}

  popd
}

function gen_gfwlist_pac() {
  pushd ${TMP_DIR}

  sed -i 's/^/    "/' ${GFWLIST_CONTENT}
  sed -i 's/$/": 1,/' ${GFWLIST_CONTENT}
  sed -i '$ s/": 1,/": 1/g' ${GFWLIST_CONTENT}

  sed -i "s/___GFWLIST_DOMAINS_PLACEHOLDER___/cat ${GFWLIST_CONTENT}/e" ${DIST_FILE}

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
extract_domains
gen_gfwlist_pac
dist_release
clean_up
