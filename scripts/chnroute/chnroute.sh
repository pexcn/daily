#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/chnroute.XXXXXX`
DIST_DIR='dist/chnroute'
DIST_FILE_IPV4='chnroute.txt'
DIST_FILE_IPV6='chnroute_v6.txt'

APNIC_URL='https://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest'
IPIP_URL='https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt'

APNIC_LIST='chnroute_apnic.txt'
IPIP_LIST='chnroute_ipip.txt'

function fetch_data() {
  pushd ${TMP_DIR}
  curl -kLs ${APNIC_URL} > ${APNIC_LIST}
  curl -kLs ${IPIP_URL} > ${IPIP_LIST}
  popd
}

function gen_ipv4_chnroute() {
  pushd ${TMP_DIR}

  local chnroute_apnic_content='chnroute_apnic_content_v4.tmp'
  local chnroute_ipip_content='chnroute_ipip_content_v4.tmp'
  local chnroute_content='chnroute_content_v4.tmp'

  cat ${APNIC_LIST} | grep ipv4 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > ${chnroute_apnic_content}
  cat ${IPIP_LIST} > ${chnroute_ipip_content}
  #cat $chnroute_apnic_content $chnroute_ipip_content | cidrmerge > $chnroute_content
  cat ${chnroute_apnic_content} ${chnroute_ipip_content} > ${chnroute_content}

  mv ${chnroute_content} ${DIST_FILE_IPV4}
  popd
}

function gen_ipv6_chnroute() {
  pushd ${TMP_DIR}

  local chnroute_content='chnroute_content_v6.tmp'

  cat ${APNIC_LIST} | grep ipv6 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > ${chnroute_content}

  mv ${chnroute_content} ${DIST_FILE_IPV6}
  popd
}

function dist_release() {
  mkdir -p ${DIST_DIR}
  mv ${TMP_DIR}/${DIST_FILE_IPV4} ${DIST_DIR}
  mv ${TMP_DIR}/${DIST_FILE_IPV6} ${DIST_DIR}
}

function clean_up() {
  rm -r ${TMP_DIR}
}

fetch_data
gen_ipv4_chnroute
gen_ipv6_chnroute
dist_release
clean_up
