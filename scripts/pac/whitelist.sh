#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/pac.XXXXXX)

DIST_FILE="dist/pac/whitelist.pac"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

function fetch_data() {
  cd $TMP_DIR

  cp $CUR_DIR/template/pac/whitelist.template .
  cp $CUR_DIR/dist/chinalist/chinalist.txt .

  cd $CUR_DIR
}

function gen_whitelist_pac() {
  cd $TMP_DIR

  local chinalist_tmp="chinalist.tmp"

  # generate content
  sed -e 's/^/  "/' -e 's/$/": 1,/' chinalist.txt |
    # remove the last line of ','
    sed '$ s/.$//g' > $chinalist_tmp

  # date
  cat <<- EOF > $DIST_NAME
	//
	// Update: $(date +'%Y-%m-%d %T')
	//

	EOF

  # replace content
  sed "s/___CHINA_DOMAINS_PLACEHOLDER___/cat $chinalist_tmp/e" whitelist.template >> $DIST_NAME

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[pac/whitelist]: OK."
}

fetch_data
gen_whitelist_pac
dist_release
clean_up
