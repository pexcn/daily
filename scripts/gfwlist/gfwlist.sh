#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/gfwlist.XXXXXX`
DIST_DIR='dist/gfwlist'
DIST_FILE='gfwlist.txt'

GFWLIST_URL='https://github.com/gfwlist/gfwlist/raw/master/gfwlist.txt'
GFWLIST='gfwlist-rules.txt'

function fetch_data() {
  local gfwlist_extras_template='template/gfwlist/gfwlist-extras.txt'

  cp ${gfwlist_extras_template} ${TMP_DIR}

  pushd ${TMP_DIR}
  curl -kLs ${GFWLIST_URL} | base64 -d > ${GFWLIST}
  popd
}

function gen_gfw_domain_list() {
  pushd ${TMP_DIR}

  local gfwlist_extras='gfwlist-extras.txt'
  local gfwlist_tmp='gfwlist.tmp'

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
      sed -r ${wildcard_pattern} > ${gfwlist_tmp}
  cat ${gfwlist_extras} >> ${gfwlist_tmp}

  cat ${gfwlist_tmp} | sort | uniq > ${DIST_FILE}

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
gen_gfw_domain_list
dist_release
clean_up
