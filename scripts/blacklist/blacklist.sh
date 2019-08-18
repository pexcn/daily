#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/blacklist.XXXXXX)

DIST_FILE="dist/blacklist/blacklist.conf"
DIST_DIR="$(dirname $DIST_FILE)"
DIST_NAME="$(basename $DIST_FILE)"

BLACKLIST_URL="https://raw.githubusercontent.com/pexcn/domain-blacklist/master/blacklist.txt"
EXCLUDE_URL="https://raw.githubusercontent.com/pexcn/domain-blacklist/master/blacklist-exclude.txt"

SERVER_DNS="114.114.114.114"

function fetch_data() {
  cd $TMP_DIR

  curl -sSL -4 --connect-timeout 10 $BLACKLIST_URL -o blacklist.txt
  curl -sSL -4 --connect-timeout 10 $EXCLUDE_URL -o blacklist-exclude.txt

  cd $CUR_DIR
}

function gen_blacklist() {
  cd $TMP_DIR

  # date
  cat <<- EOF > $DIST_NAME
	#
	# Update: $(date +'%Y-%m-%d %T')
	#

	EOF

  # blacklist
  # ignore empty lines
  sed '/^[[:space:]]*$/d' blacklist.txt |
    # convert to dnsmasq format
    sed -e '/^#/!s/^/server=\//g' -e '/^#/!s/$/\//g' >> $DIST_NAME

  # dividing line
  echo >> $DIST_NAME

  # excludes
  # ignore empty lines
  sed '/^[[:space:]]*$/d' blacklist-exclude.txt |
    # convert to dnsmasq format
    sed -e '/^#/!s/^/server=\//g' -e "/^#/!s/$/\/$SERVER_DNS/g" >> $DIST_NAME

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_NAME $DIST_FILE
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[blacklist]: OK."
}

fetch_data
gen_blacklist
dist_release
clean_up
