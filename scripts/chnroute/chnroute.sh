#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/chnroute.XXXXXX)

SRC_URL_1="https://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest"
SRC_URL_2="https://raw.githubusercontent.com/17mon/china_ip_list/master/china_ip_list.txt"
DEST_FILE_1="dist/chnroute/chnroute.txt"
DEST_FILE_2="dist/chnroute/chnroute-v6.txt"

fetch_src() {
  cd $TMP_DIR

  curl -sSL $SRC_URL_1 -o apnic.txt
  curl -sSL $SRC_URL_2 -o ipip.txt

  cd $CUR_DIR
}

gen_list_v4() {
  cd $TMP_DIR

  # convert to cidr format
  cat apnic.txt | grep ipv4 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > apnic.tmp
  # add newline to end of file only if newline doesn't exist
  sed '$a\' ipip.txt > ipip.tmp
  # ipv4 cidr merge
  cat apnic.tmp ipip.tmp | $CUR_DIR/tools/ip-dedup/obj/ip-dedup -4 > chnroute.txt

  cd $CUR_DIR
}

gen_list_v6() {
  cd $TMP_DIR

  # convert to cidr format
  cat apnic.txt | grep ipv6 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, $5) }' > apnic6.tmp
  # ipv6 cidr merge
  $CUR_DIR/tools/ip-dedup/obj/ip-dedup -6 < apnic6.tmp > chnroute-v6.txt

  cd $CUR_DIR
}

copy_dest() {
  install -D $TMP_DIR/chnroute.txt $DEST_FILE_1
  install -D $TMP_DIR/chnroute-v6.txt $DEST_FILE_2
}

clean_up() {
  rm -r $TMP_DIR
  echo "[$(basename $0 .sh)]: done."
}

fetch_src
gen_list_v4
gen_list_v6
copy_dest
clean_up
