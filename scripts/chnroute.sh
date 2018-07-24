#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/chnroute.XXXXXX`
DIST_DIR='dist/chnroute'

APNIC_URL='https://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest'
IPIP_URL='https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt'

function fetch_data() {
  pushd $TMP_DIR
  curl -kLs $APNIC_URL > chnroute_apnic.txt
  curl -kLs $IPIP_URL > chnroute_ipip.txt
  popd
}

function gen_ipv4_chnroute() {
  pushd $TMP_DIR

  cat chnroute_apnic.txt | grep ipv4 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > chnroute_apnic.tmp
  cat chnroute_ipip.txt > chnroute_ipip.tmp
  cat chnroute_apnic.tmp chnroute_ipip.tmp | cidrmerge > chnroute.tmp

  mv chnroute.tmp chnroute.txt
  popd
}

function gen_ipv6_chnroute() {
  pushd $TMP_DIR

  cat chnroute_apnic.txt | grep ipv6 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > chnroute_v6.tmp

  mv chnroute_v6.tmp chnroute_v6.txt
  popd
}

function dist_release() {
  mkdir -p $DIST_DIR
  mv $TMP_DIR/chnroute.txt $DIST_DIR
  mv $TMP_DIR/chnroute_v6.txt $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
}

fetch_data
gen_ipv4_chnroute
gen_ipv6_chnroute
dist_release
clean_up
