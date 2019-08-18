#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/shadowrocket.XXXXXX)

DIST_FILE="dist/shadowrocket/whitelist.conf"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

function fetch_data() {
  cd $TMP_DIR

  cp $CUR_DIR/template/shadowrocket/whitelist.template .
  cp $CUR_DIR/dist/chinalist/chinalist.txt .

  cd $CUR_DIR
}

function gen_whitelist_config() {
  cd $TMP_DIR

  local chinalist_tmp="chinalist.tmp"

  # limit 2000 entries
  head -2000 chinalist.txt |
    # generate content
    sed -e 's/^/DOMAIN-SUFFIX,/' -e 's/$/,DIRECT/' > $chinalist_tmp

  # date
  cat <<- EOF > $DIST_NAME
	#
	# Update: $(date +'%Y-%m-%d %T')
	#

	EOF

  # replace content
  sed "s/___WHITELIST_DOMAINS_PLACEHOLDER___/cat $chinalist_tmp/e" whitelist.template >> $DIST_NAME

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[shadowrocket/whitelist]: OK."
}

fetch_data
gen_whitelist_config
dist_release
clean_up
