#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/pac.XXXXXX`
DIST_DIR='dist/pac'
DIST_FILE='gfwlist.pac'

PAC_TEMPLATE='template/pac/gfwlist.pac'
EXTRA_DOMAINS_TEMPLATE='template/gfwlist_extra_domains.txt'

GFWLIST_URL='https://github.com/gfwlist/gfwlist/raw/master/gfwlist.txt'
GFWLIST='gfwlist.txt'

function fetch_data() {
  cp ${PAC_TEMPLATE} ${TMP_DIR}
  cp ${EXTRA_DOMAINS_TEMPLATE} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${GFWLIST_URL} | base64 -d > ${GFWLIST}
  popd
}

function gen_gfwlist_pac() {
  pushd ${TMP_DIR}

  local gfwlist_content='gfwlist_content.tmp'
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
      sed -r ${wildcard_pattern} > ${gfwlist_content}
  cat ${gfwlist_extra_domains} >> ${gfwlist_content}

  sort -u ${gfwlist_content} -o ${gfwlist_content}

  sed -i 's/^/    "/' ${gfwlist_content}
  sed -i 's/$/": 1,/' ${gfwlist_content}
  sed -i '$ s/": 1,/": 1/g' ${gfwlist_content}

  sed -i "s/___GFWLIST_DOMAINS_PLACEHOLDER___/cat ${gfwlist_content}/e" ${DIST_FILE}

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
gen_gfwlist_pac
dist_release
clean_up
