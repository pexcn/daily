#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/adblock.XXXXXX`
DIST_DIR='dist/adblock'

EASYLIST_URL='https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt'
ABPFX_URL='https://raw.githubusercontent.com/xinggsf/Adblock-Plus-Rule/master/ABP-FX.txt'
YOYO_URL='https://pgl.yoyo.org/adservers/serverlist.php?hostformat=hosts&showintro=0&mimetype=plaintext'
ADAWAY_URL='https://hosts-file.net/ad_servers.txt'

function fetch_data() {
  pushd $TMP_DIR
  curl -kLs $EASYLIST_URL > easylistchina.txt
  curl -kLs $ABPFX_URL > abp-fx.txt
  curl -kLs $YOYO_URL > yoyo.txt
  curl -kLs $ADAWAY_URL > adaway.txt
  popd
}

function gen_adblock_rules() {
  pushd $TMP_DIR

  cat easylistchina.txt | grep ^\|\|[^\*]*\^$ | sed -e 's:||:address\=\/:' -e 's:\^:/127\.0\.0\.1:' > easylistchina.tmp
  cat abp-fx.txt | grep ^\|\|[^\*]*\^$ | sed -e 's:||:address\=\/:' -e 's:\^:/127\.0\.0\.1:' > abp-fx.tmp
  cat yoyo.txt | grep -E '^127.0.0.1' | awk '{printf "address=/%s/127.0.0.1\n",$2}' > yoyo.tmp
  cat adaway.txt | sed  $'s/\r$//' | grep -E '^127.0.0.1' | awk '{printf "address=/%s/127.0.0.1\n",$2}' > adaway.tmp

  cat easylistchina.tmp abp-fx.tmp yoyo.tmp | sort | uniq > adblock.conf
  cat easylistchina.tmp abp-fx.tmp yoyo.tmp adaway.tmp | sort | uniq > adblock_full.conf

  sed -i "1i#\n# Update: $(date +'%Y-%m-%d %T')\n#\n" adblock.conf adblock_full.conf
  popd
}

function dist_release() {
  mkdir -p $DIST_DIR
  mv $TMP_DIR/adblock.conf $DIST_DIR
  mv $TMP_DIR/adblock_full.conf $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
}

fetch_data
gen_adblock_rules
dist_release
clean_up
