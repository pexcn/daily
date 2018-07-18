#!/bin/bash -e

# chnroute
function gen_chnroute() {
  # ipv4 chnroute
  mkdir -p gen/chnroute
  pushd gen/chnroute
  curl -kL 'http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest' | grep ipv4 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > chnroute.txt.tmp
  curl -kL 'https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt' > chnroute_ipip.txt.tmp
  cat chnroute.txt.tmp chnroute_ipip.txt.tmp | cidrmerge > chnroute.txt
  popd

  # ipv6 chnroute
  mkdir -p gen/chnroute
  pushd gen/chnroute
  curl -kL 'http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest' | grep ipv6 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > chnroute_v6.txt.tmp
  mv chnroute_v6.txt.tmp chnroute_v6.txt
  popd
}

# dnsmasq rules
function gen_dnsmasq_rules() {
  mkdir -p gen/dnsmasq
  pushd gen/dnsmasq

  curl -kL 'https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt' | grep ^\|\|[^\*]*\^$ | sed -e 's:||:address\=\/:' -e 's:\^:/127\.0\.0\.1:' > easylistchina.conf.tmp
  curl -kL 'https://raw.githubusercontent.com/xinggsf/Adblock-Plus-Rule/master/ABP-FX.txt' | grep ^\|\|[^\*]*\^$ | sed -e 's:||:address\=\/:' -e 's:\^:/127\.0\.0\.1:' > abp-fx.conf.tmp
  curl -kL 'https://pgl.yoyo.org/adservers/serverlist.php?hostformat=hosts&showintro=0&mimetype=plaintext' | grep -E '^127.0.0.1' | awk '{printf "address=/%s/127.0.0.1\n",$2}' > yoyo.conf.tmp
  curl -kL 'https://hosts-file.net/ad_servers.txt' | sed  $'s/\r$//' | grep -E '^127.0.0.1' | awk '{printf "address=/%s/127.0.0.1\n",$2}' > adaway.conf.tmp

  cat easylistchina.conf.tmp abp-fx.conf.tmp yoyo.conf.tmp | sort | uniq > adblock.conf
  cat easylistchina.conf.tmp abp-fx.conf.tmp yoyo.conf.tmp adaway.conf.tmp | sort | uniq > adblock_full.conf

  sed -i "1i#\n# Update: $(date +'%Y-%m-%d %T')\n#\n" adblock.conf adblock_full.conf
  popd
}

# TODO: enhance readability
# gfw whitelist pac
function gen_whitelist_pac() {
  mkdir -p gen/pac
  pushd gen/pac

  curl -kL 'https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf' > china_domain_list.tmp

  # get domains
  sed -i 's/server=\//"/g' china_domain_list.tmp
  sed -i 's/\/114.114.114.114/":1,/g' china_domain_list.tmp

  # remove comments, these domains can be directly connected in China
  sed -i 's/#//g' china_domain_list.tmp
  # if above domains cannot be connected in China, should be remove include '#' lines
  #sed -i '/#/d' china_domain_list.tmp

  # remove last ',' character via: https://stackoverflow.com/questions/3576139/sed-remove-string-only-in-the-last-line-of-the-file
  sed -i '$ s/":1,/":1/g' china_domain_list.tmp

  sed 's/__CHINA_DOMAIN_LIST_PLACEHOLDER__/cat china_domain_list.tmp/e' ../../template/whitelist.pac > whitelist.pac

  sed -i "1i//\n// Update: $(date +'%Y-%m-%d %T')\n//\n" whitelist.pac
  popd
}

function clean_up() {
  rm gen/chnroute/chnroute.txt.tmp
  rm gen/chnroute/chnroute_ipip.txt.tmp

  rm gen/dnsmasq/easylistchina.conf.tmp
  rm gen/dnsmasq/abp-fx.conf.tmp
  rm gen/dnsmasq/yoyo.conf.tmp
  rm gen/dnsmasq/adaway.conf.tmp

  rm gen/pac/china_domain_list.tmp
}

function dist_release() {
  cp -r gen dist
}

gen_chnroute
gen_dnsmasq_rules
gen_whitelist_pac

clean_up
dist_release
