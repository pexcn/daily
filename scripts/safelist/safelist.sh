#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/safelist.XXXXXX)

DIST_FILE="dist/safelist/safelist.conf"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

# format: IPV4_ADDRESS[#PORT]
TRUST_DNS_SERVER="127.0.0.1#5300"

function fetch_data() {
  cd $TMP_DIR

  cp $CUR_DIR/dist/gfwlist/gfwlist.txt warnlist.txt

  cd $CUR_DIR
}

function gen_safelist() {
  cd $TMP_DIR

  # date
  cat <<- EOF > $DIST_NAME
	#
	# Update: $(date +'%Y-%m-%d %T')
	#

	EOF

  # convert to dnsmasq format
  sed -e "s/^/server=\//" -e "s/$/\/$TRUST_DNS_SERVER/" warnlist.txt >> $DIST_NAME

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[safelist]: OK."
}

fetch_data
gen_safelist
dist_release
clean_up
