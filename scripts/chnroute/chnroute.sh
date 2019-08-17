#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/chnroute.XXXXXX)
DIST_DIR="$CUR_DIR/dist/chnroute"
DIST_FILE_IPV4="chnroute.txt"
DIST_FILE_IPV6="chnroute-v6.txt"

APNIC_URL="https://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest"
IPIP_URL="https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt"
APNIC_LIST="apnic.txt"
IPIP_LIST="ipip.txt"

function fetch_data() {
  cd $TMP_DIR

  curl -sSL $APNIC_URL > $APNIC_LIST
  curl -sSL $IPIP_URL > $IPIP_LIST

  cd $CUR_DIR
}

function gen_ipv4_chnroute() {
  cd $TMP_DIR

  local apnic_v4="apnic_v4.tmp"
  local ipip_v4="ipip_v4.tmp"
  local chnroute_v4="chnroute_v4.tmp"

  cat $APNIC_LIST | grep ipv4 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > $apnic_v4
  cat $IPIP_LIST > $ipip_v4
  cat $apnic_v4 $ipip_v4 | aggregate -q > $chnroute_v4

  mv $chnroute_v4 $DIST_FILE_IPV4

  cd $CUR_DIR
}

function gen_ipv6_chnroute() {
  cd $TMP_DIR

  local apnic_v6="apnic_v6.tmp"

  cat $APNIC_LIST | grep ipv6 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, $5) }' > ${apnic_v6}

  mv $apnic_v6 $DIST_FILE_IPV6

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_IPV4 $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_IPV6 $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[chnroute]: OK."
}

fetch_data
gen_ipv4_chnroute
gen_ipv6_chnroute
dist_release
clean_up
