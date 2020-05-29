#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/tldlist.XXXXXX)

SRC_URL_1="https://data.iana.org/TLD/tlds-alpha-by-domain.txt"
SRC_URL_2="https://raw.githubusercontent.com/publicsuffix/list/master/public_suffix_list.dat"
DEST_FILE="dist/tldlist/tldlist.txt"

gen_list() {
  cd $TMP_DIR

  # iana
  curl -sSL $SRC_URL_1 |
    # remove comment lines
    sed '/^#/ d' |
    # remove empty lines
    tr '[:upper:]' '[:lower:]' > iana-tlds.txt

  # publicsuffix
  curl -sSL $SRC_URL_2 |
    # remove comment lines
    sed '/^\/\// d' |
    # remove empty lines
    sed '/^[[:space:]]*$/d' |
    # only allow ascii characters
    grep -P '^[[:ascii:]]+$' |
    # must have only 1 dot
    grep -E '^[^\.]*\.[^\.]*$' |
    # do not start with * or !
    grep -v '^*\|^!' |
    # limit domain length
    awk -F '.' 'length($1) >= 2 && length($1) <= 3 && length($2) <= 3' > publicsuffix-tlds.txt

  # merge to tldlist
  cat iana-tlds.txt publicsuffix-tlds.txt | awk '!x[$0]++' > tldlist.txt

  cd $CUR_DIR
}

copy_dest() {
  install -D -m 644 $TMP_DIR/tldlist.txt $DEST_FILE
}

clean_up() {
  rm -r $TMP_DIR
}

gen_list
copy_dest
clean_up
