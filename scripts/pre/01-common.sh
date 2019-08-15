#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/alexa.XXXXXX)
DIST_DIR="$CUR_DIR/dist/alexa"
DIST_FILE_TOP="top.txt"
DIST_FILE_TOP_CN="top-cn.txt"

ALEXA_LIST_URL="https://s3.amazonaws.com/alexa-static/top-1m.csv.zip"
CHINA_LIST_SRC="$CUR_DIR/dist/chinalist/chinalist.txt"
ALEXA_LIST="alexalist.txt"
CHINA_LIST=$(basename $CHINA_LIST_SRC)

function fetch_data() {
  cd $TMP_DIR

  curl -sSL $ALEXA_LIST_URL | gunzip | awk -F ',' '{print $2}' > $ALEXA_LIST
  cp $CHINA_LIST_SRC $CHINA_LIST

  cd $CUR_DIR
}

function gen_alexa_top_list() {
  cd $TMP_DIR

  cp $ALEXA_LIST $DIST_FILE_TOP
  grep -Fx -f $CHINA_LIST $ALEXA_LIST > $DIST_FILE_TOP_CN

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_TOP $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_TOP_CN $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
  echo "[alexa]: OK."
}

fetch_data
gen_alexa_top_list
dist_release
clean_up
