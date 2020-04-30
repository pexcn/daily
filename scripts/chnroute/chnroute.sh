#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/chnroute.XXXXXX)

DIST_FILE_IPV4="dist/chnroute/chnroute.txt"
DIST_FILE_IPV6="dist/chnroute/chnroute-v6.txt"
DIST_DIR="$(dirname $DIST_FILE_IPV4)"
DIST_NAME_IPV4="$(basename $DIST_FILE_IPV4)"
DIST_NAME_IPV6="$(basename $DIST_FILE_IPV6)"

APNIC_URL="https://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest"
IPIP_URL="https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt"
APNIC_LIST="apnic.txt"
IPIP_LIST="ipip.txt"

fetch_data() {
  cd $TMP_DIR

  curl -sSL -4 --connect-timeout 10 $APNIC_URL -o apnic.txt
  curl -sSL -4 --connect-timeout 10 $IPIP_URL -o ipip.txt

  cd $CUR_DIR
}

gen_ipv4_chnroute() {
  cd $TMP_DIR

  local apnic_tmp="apnic.tmp"
  local ipip_tmp="ipip.tmp"

  cat apnic.txt | grep ipv4 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > $apnic_tmp
  cat ipip.txt > $ipip_tmp
  cat $apnic_tmp $ipip_tmp | aggregate -q > $DIST_NAME_IPV4

  cd $CUR_DIR
}

gen_ipv6_chnroute() {
  cd $TMP_DIR

  cat apnic.txt | grep ipv6 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, $5) }' > $DIST_NAME_IPV6

  cd $CUR_DIR
}

dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME_IPV4 $DIST_FILE_IPV4
  cp $TMP_DIR/$DIST_NAME_IPV6 $DIST_FILE_IPV6
}

clean_up() {
  rm -r $TMP_DIR
  echo "[chnroute]: OK."
}

fetch_data
gen_ipv4_chnroute
gen_ipv6_chnroute
dist_release
clean_up
