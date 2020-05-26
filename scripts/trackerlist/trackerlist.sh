#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/trackerlist.XXXXXX)

SRC_URL_1="https://ngosang.github.io/trackerslist/trackers_all.txt"
SRC_URL_2="https://ngosang.github.io/trackerslist/trackers_best.txt"
SRC_URL_3="https://raw.githubusercontent.com/XIU2/TrackersListCollection/master/all.txt"
SRC_URL_4="https://raw.githubusercontent.com/XIU2/TrackersListCollection/master/best.txt"
DEST_FILE_1="dist/trackerlist/trackerlist.txt"
DEST_FILE_2="dist/trackerlist/trackerlist-best.txt"
DEST_FILE_3="dist/trackerlist/trackerlist-aria2.txt"
DEST_FILE_4="dist/trackerlist/trackerlist-best-aria2.txt"

fetch_src() {
  cd $TMP_DIR

  curl -sSL $SRC_URL_1 -o all-1.txt
  curl -sSL $SRC_URL_2 -o best-1.txt
  curl -sSL $SRC_URL_3 -o all-2.txt
  curl -sSL $SRC_URL_4 -o best-2.txt

  cd $CUR_DIR
}

gen_list() {
  cd $TMP_DIR

  # add newline to end of file only if newline doesn't exist
  sed -i '$a\' all-1.txt all-2.txt best-1.txt best-2.txt

  # remove empty lines
  sed -i '/^[[:space:]]*$/d' all-1.txt all-2.txt best-1.txt best-2.txt

  # remove duplicates without sorting
  awk '!x[$0]++' all-1.txt all-2.txt > all.txt
  awk '!x[$0]++' best-1.txt best-2.txt > best.txt

  # aria2 version
  cat all.txt | xargs echo -n | tr ' ' ',' > all-aria2.txt
  cat best.txt | xargs echo -n | tr ' ' ',' > best-aria2.txt

  cd $CUR_DIR
}

copy_dest() {
  install -D $TMP_DIR/all.txt $DEST_FILE_1
  install -D $TMP_DIR/best.txt $DEST_FILE_2
  install -D $TMP_DIR/all-aria2.txt $DEST_FILE_3
  install -D $TMP_DIR/best-aria2.txt $DEST_FILE_4
}

clean_up() {
  rm -r $TMP_DIR
  echo "[$(basename $0 .sh)]: done."
}

fetch_src
gen_list
copy_dest
clean_up
