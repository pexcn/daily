#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/adlist.XXXXXX`
DIST_DIR='dist/adlist'
DIST_FILE='adlist.txt'
DIST_FILE_FULL='adlist-full.txt'

EASYLIST_URL='https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt'
ABPFX_URL='https://raw.githubusercontent.com/xinggsf/Adblock-Plus-Rule/master/ABP-FX.txt'
YOYO_URL='https://pgl.yoyo.org/adservers/serverlist.php?hostformat=hosts&showintro=0&mimetype=plaintext'
ADAWAY_URL='https://hosts-file.net/ad_servers.txt'

EASY_LIST='easylistchina.txt'
ABPFX_LIST='abpfx.txt'
YOYO_LIST='yoyo.txt'
ADAWAY_LIST='adaway.txt'

function fetch_data() {
  pushd ${TMP_DIR}
  curl -kLs ${EASYLIST_URL} > ${EASY_LIST}
  curl -kLs ${ABPFX_URL} > ${ABPFX_LIST}
  curl -kLs ${YOYO_URL} > ${YOYO_LIST}
  curl -kLs ${ADAWAY_URL} > ${ADAWAY_LIST}
  popd
}

function gen_adlist() {
  pushd ${TMP_DIR}

  local easylist_content='easylistchina_content.tmp'
  local abpfx_content='abpfx_content.tmp'
  local yoyo_content='yoyo_content.tmp'
  local adaway_content='adaway_content.tmp'

  cat ${EASY_LIST} | grep ^\|\|[^\*]*\^$ | sed -e 's:||::' -e 's:\^::' > ${easylist_content}
  cat ${ABPFX_LIST} | grep ^\|\|[^\*]*\^$ | sed -e 's:||::' -e 's:\^::' > ${abpfx_content}
  cat ${YOYO_LIST} | grep -E '^127.0.0.1' | sed "s:127.0.0.1 ::" > ${yoyo_content}
  cat ${ADAWAY_LIST} | sed  $'s/\r$//' | grep -E '^127.0.0.1' | grep -v '#' | sed "s:127.0.0.1\t::" > ${adaway_content}

  cat ${easylist_content} ${abpfx_content} ${yoyo_content} | sort | uniq > ${DIST_FILE}
  cat ${easylist_content} ${abpfx_content} ${yoyo_content} ${adaway_content} | sort | uniq > ${DIST_FILE_FULL}

  popd
}

function dist_release() {
  mkdir -p ${DIST_DIR}
  mv ${TMP_DIR}/${DIST_FILE} ${DIST_DIR}
  mv ${TMP_DIR}/${DIST_FILE_FULL} ${DIST_DIR}
}

function clean_up() {
  rm -r ${TMP_DIR}
}

fetch_data
gen_adlist
dist_release
clean_up
