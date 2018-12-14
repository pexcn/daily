#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/adblock.XXXXXX`
DIST_DIR='dist/adblock'
DIST_FILE='adblock.conf'

ADLIST_URL='https://raw.githubusercontent.com/pexcn/daily/gh-pages/adlist/adlist.txt'
ADLIST='adlist.txt'

function fetch_data() {
  pushd ${TMP_DIR}
  curl -kLs ${ADLIST_URL} > ${ADLIST}
  popd
}

function gen_adblock_rules() {
  pushd ${TMP_DIR}

  sed -e 's/^/address=\//' -e 's/$/\/127.0.0.1/' ${ADLIST} > ${DIST_FILE}
  sed -i "1i#\n# Update: $(date +'%Y-%m-%d %T')\n#\n" ${DIST_FILE}

  popd
}

function dist_release() {
  mkdir -p ${DIST_DIR}
  mv ${TMP_DIR}/${DIST_FILE} ${DIST_DIR}
}

function clean_up() {
  rm -r ${TMP_DIR}
}

fetch_data
gen_adblock_rules
dist_release
clean_up
