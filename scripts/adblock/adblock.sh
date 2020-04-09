#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/adblock.XXXXXX)

DIST_FILE="dist/adblock/adblock.conf"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

function fetch_data() {
  cd $TMP_DIR

  cp $CUR_DIR/dist/adlist/adlist.txt .

  cd $CUR_DIR
}

function gen_adblock_list() {
  cd $TMP_DIR

  local ipv4_regex="[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}"

  # date
  cat <<- EOF > $DIST_NAME
	#
	# Update: $(date +'%Y-%m-%d %T')
	#

	EOF

  cat adlist.txt |
    # convert to dnsmasq format
    sed -e "s/^/server=\//" -e "s/$/\//" |
	# ignore ipv4 address
    grep -v "$ipv4_regex" >> $DIST_NAME

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[adblock]: OK."
}

fetch_data
gen_adblock_list
dist_release
clean_up
