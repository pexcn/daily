#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/blacklist.XXXXXX)
DIST_DIR="$CUR_DIR/dist/blacklist"
DIST_FILE="blacklist.conf"

BLACK_LIST_URL="https://raw.githubusercontent.com/pexcn/domain-blacklist/master/blacklist.txt"
EXCLUDE_LIST_URL="https://raw.githubusercontent.com/pexcn/domain-blacklist/master/blacklist-exclude.txt"

BLACK_LIST=$(basename $BLACK_LIST_URL)
EXCLUDE_LIST=$(basename $EXCLUDE_LIST_URL)

SERVER_DNS="114.114.114.114"

function fetch_data() {
  cd $TMP_DIR

  curl -sSL $BLACK_LIST_URL > $BLACK_LIST
  curl -sSL $EXCLUDE_LIST_URL > $EXCLUDE_LIST

  cd $CUR_DIR
}

function gen_blacklist() {
  cd $TMP_DIR

  # date tag
  cat << EOF > $DIST_FILE
#
# Update: $(date +'%Y-%m-%d %T')
#

EOF

  # blacklist
  cat << EOF >> $DIST_FILE
#
# Block the harmful domains
#

EOF
  sed '/^[[:space:]]*$/d' $BLACK_LIST | sed -e '/^#/!s/^/server=\//g' -e '/^#/!s/$/\//g' >> $DIST_FILE

  # empty lines
  echo >> $DIST_FILE
  echo >> $DIST_FILE
  echo >> $DIST_FILE

  # excludes
  cat << EOF >> $DIST_FILE
#
# Exclude some subdomains
#

EOF
  sed '/^[[:space:]]*$/d' $EXCLUDE_LIST | sed -e '/^#/!s/^/server=\//g' -e "/^#/!s/$/\/$SERVER_DNS/g" >> $DIST_FILE

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[blacklist]: OK."
}

fetch_data
gen_blacklist
dist_release
clean_up
