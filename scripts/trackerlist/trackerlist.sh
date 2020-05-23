#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/trackerlist.XXXXXX)

DIST_DIR="dist/trackerlist"
DIST_FILE_ALL="dist/trackerlist/all.txt"
DIST_FILE_BEST="dist/trackerlist/best.txt"

DIST_DIR_ARIA2="dist/trackerlist/aria2"
DIST_FILE_ALL_ARIA2="dist/trackerlist/aria2/all.txt"
DIST_FILE_BEST_ARIA2="dist/trackerlist/aria2/best.txt"

TRACKER_URL_ALL_1="https://ngosang.github.io/trackerslist/trackers_all.txt"
TRACKER_URL_ALL_2="https://raw.githubusercontent.com/XIU2/TrackersListCollection/master/all.txt"
TRACKER_URL_BEST_1="https://ngosang.github.io/trackerslist/trackers_best.txt"
TRACKER_URL_BEST_2="https://raw.githubusercontent.com/XIU2/TrackersListCollection/master/best.txt"

fetch_data() {
  cd $TMP_DIR

  curl -sSL --connect-timeout 10 $TRACKER_URL_ALL_1 -o all_1.txt
  curl -sSL --connect-timeout 10 $TRACKER_URL_ALL_2 -o all_2.txt
  curl -sSL --connect-timeout 10 $TRACKER_URL_BEST_1 -o best_1.txt
  curl -sSL --connect-timeout 10 $TRACKER_URL_BEST_2 -o best_2.txt

  cd $CUR_DIR
}

gen_trackerlist() {
  cd $TMP_DIR

  # remove empty lines & add newline to end of file
  sed -e '/^$/d' -e '$a\' -i all_*.txt best_*.txt

  # remove duplicates without sorting
  awk '!x[$0]++' all_*.txt > all.txt
  awk '!x[$0]++' best_*.txt > best.txt

  # aria2 version
  cat all.txt | xargs echo -n | tr ' ' ',' > all.aria2
  cat best.txt | xargs echo -n | tr ' ' ',' > best.aria2

  cd $CUR_DIR
}

dist_release() {
  mkdir -p $DIST_DIR $DIST_DIR_ARIA2
  cp $TMP_DIR/all.txt $DIST_FILE_ALL
  cp $TMP_DIR/best.txt $DIST_FILE_BEST
  cp $TMP_DIR/all.aria2 $DIST_FILE_ALL_ARIA2
  cp $TMP_DIR/best.aria2 $DIST_FILE_BEST_ARIA2
}

clean_up() {
  rm -r $TMP_DIR
  echo "[trackerlist]: OK."
}

fetch_data
gen_trackerlist
dist_release
clean_up
