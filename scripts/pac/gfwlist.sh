#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/pac.XXXXXX)

DIST_FILE="dist/pac/gfwlist.pac"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

fetch_data() {
  cd $TMP_DIR

  cp $CUR_DIR/template/pac/gfwlist.template .
  cp $CUR_DIR/dist/gfwlist/gfwlist.txt .

  cd $CUR_DIR
}

gen_gfwlist_pac() {
  cd $TMP_DIR

  local gfwlist_tmp="gfwlist.tmp"

  # generate content
  sed -e 's/^/  "/' -e 's/$/": 1,/' gfwlist.txt |
    # remove the last line of ','
    sed '$ s/.$//g' > $gfwlist_tmp

  # date
  cat <<- EOF > $DIST_NAME
	//
	// Update: $(date +'%Y-%m-%d %T')
	//

	EOF

  # replace content
  sed "s/___GFWLIST_DOMAINS_PLACEHOLDER___/cat $gfwlist_tmp/e" gfwlist.template >> $DIST_NAME

  cd $CUR_DIR
}

dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

clean_up() {
  rm -r $TMP_DIR
  echo "[pac/gfwlist]: OK."
}

fetch_data
gen_gfwlist_pac
dist_release
clean_up
