#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/adblock.XXXXXX`
DIST_DIR='dist/adblock'
DIST_FILE='adblock.conf'

EASYLIST_URL='https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt'
ABPFX_URL='https://raw.githubusercontent.com/xinggsf/Adblock-Plus-Rule/master/ABP-FX.txt'
YOYO_URL='https://pgl.yoyo.org/adservers/serverlist.php?hostformat=hosts&showintro=0&mimetype=plaintext'

EASY_LIST='easylistchina.txt'
ABPFX_LIST='abpfx.txt'
YOYO_LIST='yoyo.txt'

function fetch_data() {
  pushd ${TMP_DIR}
  curl -kLs ${EASYLIST_URL} > ${EASY_LIST}
  curl -kLs ${ABPFX_URL} > ${ABPFX_LIST}
  curl -kLs ${YOYO_URL} > ${YOYO_LIST}
  popd
}

function gen_adblock_rules() {
  pushd ${TMP_DIR}

  local easylist_content='easylistchina_content.tmp'
  local abpfx_content='abpfx_content.tmp'
  local yoyo_content='yoyo_content.tmp'

  cat ${EASY_LIST} | grep ^\|\|[^\*]*\^$ | sed -e 's:||:address\=\/:' -e 's:\^:/127\.0\.0\.1:' > ${easylist_content}
  cat ${ABPFX_LIST} | grep ^\|\|[^\*]*\^$ | sed -e 's:||:address\=\/:' -e 's:\^:/127\.0\.0\.1:' > ${abpfx_content}
  cat ${YOYO_LIST} | grep -E '^127.0.0.1' | awk '{printf "address=/%s/127.0.0.1\n",$2}' > ${yoyo_content}

  cat ${easylist_content} ${abpfx_content} ${yoyo_content} | sort | uniq > ${DIST_FILE}

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
