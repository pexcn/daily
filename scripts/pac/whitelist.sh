#!/bin/bash -e

TMP_DIR=`mktemp -d /tmp/pac.XXXXXX`
DIST_DIR='dist/pac'

CHINA_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf'
APPLE_DOMAINS_URL='https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf'

function fetch_data() {
  cp template/pac/whitelist.pac $TMP_DIR

  pushd $TMP_DIR
  curl -kLs $CHINA_DOMAINS_URL > china_domains.txt
  curl -kLs $APPLE_DOMAINS_URL > apple_domains.txt
  popd
}

function gen_whitelist_pac() {
  pushd $TMP_DIR

  cat china_domains.txt apple_domains.txt > whitelist_domains.tmp

  # get domains
  sed -i 's/server=\//    "/g' whitelist_domains.tmp
  sed -i 's/\/114.114.114.114/": 1,/g' whitelist_domains.tmp

  # exclude comment lines
  sed -i '/#/d' whitelist_domains.tmp

  # remove last ',' character: https://stackoverflow.com/questions/3576139/sed-remove-string-only-in-the-last-line-of-the-file
  sed -i '$ s/": 1,/": 1/g' whitelist_domains.tmp

  sed -i 's/___CHINA_DOMAINS_PLACEHOLDER___/cat whitelist_domains.tmp/e' whitelist.pac

  sed -i "1i//\n// Update: $(date +'%Y-%m-%d %T')\n//\n" whitelist.pac
  popd
}

function dist_release() {
  mkdir -p $DIST_DIR
  mv $TMP_DIR/whitelist.pac $DIST_DIR
}

function clean_up() {
  rm -r $TMP_DIR
}

fetch_data
gen_whitelist_pac
dist_release
clean_up
