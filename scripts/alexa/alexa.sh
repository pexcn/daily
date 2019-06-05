#!/bin/bash -e

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/alexa.XXXXXX)
DIST_DIR="$CUR_DIR/dist/alexa"
DIST_FILE_TOP_5000="top-5000.txt"
DIST_FILE_TOP_2000="top-2000.txt"
DIST_FILE_TOP_500="top-500.txt"
DIST_FILE_TOP_CN_5000="top-cn-5000.txt"
DIST_FILE_TOP_CN_2000="top-cn-2000.txt"
DIST_FILE_TOP_CN_500="top-cn-500.txt"

ALEXA_LIST_URL="https://s3.amazonaws.com/alexa-static/top-1m.csv.zip"
CHINA_LIST_URL="https://raw.githubusercontent.com/pexcn/daily/gh-pages/chinalist/chinalist.txt"
ALEXA_LIST="alexalist.txt"
CHINA_LIST="chinalist.txt"

function fetch_data() {
  cd $TMP_DIR

  curl -sSL $ALEXA_LIST_URL | gunzip | awk -F ',' '{print $2}' > $ALEXA_LIST
  curl -sSL $CHINA_LIST_URL > $CHINA_LIST

  cd $CUR_DIR
}

function gen_alexa_top_list() {
  cd $TMP_DIR

  local top=$ALEXA_LIST
  local top_cn="top-cn.txt"

  head -5000 $top > $DIST_FILE_TOP_5000
  head -2000 $top > $DIST_FILE_TOP_2000
  head -500 $top > $DIST_FILE_TOP_500

  grep -Fx -f $CHINA_LIST $ALEXA_LIST > $top_cn
  head -5000 $top_cn > $DIST_FILE_TOP_CN_5000
  head -2000 $top_cn > $DIST_FILE_TOP_CN_2000
  head -500 $top_cn > $DIST_FILE_TOP_CN_500

  cd $CUR_DIR
}

function dist_release() {
  mkdir -p $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_TOP_5000 $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_TOP_2000 $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_TOP_500 $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_TOP_CN_5000 $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_TOP_CN_2000 $DIST_DIR
  cp $TMP_DIR/$DIST_FILE_TOP_CN_500 $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
}

fetch_data
gen_alexa_top_list
dist_release
#clean_up
