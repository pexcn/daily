#!/bin/bash -e

# chnroute
function gen_chnroute() {
  # ipv4
  mkdir -p gen/chnroute
  pushd gen/chnroute
  curl -kL 'http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest' | grep ipv4 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > chnroute.txt.tmp
  curl -kL 'https://github.com/17mon/china_ip_list/raw/master/china_ip_list.txt' > chnroute_ipip.txt.tmp
  cat chnroute.txt.tmp chnroute_ipip.txt.tmp | cidrmerge > chnroute.txt
  popd

  # ipv6
  mkdir -p gen/chnroute
  pushd gen/chnroute
  curl -kL 'http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest' | grep ipv6 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > chnroute_v6.txt.tmp
  mv chnroute_v6.txt.tmp chnroute_v6.txt
  popd
}

# ad blocking rules
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

# gfw whitelist pac
function gen_whitelist_pac() {
  mkdir -p gen/pac
  pushd gen/pac

  curl -kL 'https://github.com/felixonmars/dnsmasq-china-list/raw/master/accelerated-domains.china.conf' > china_domain_list.tmp
  curl -kL 'https://github.com/felixonmars/dnsmasq-china-list/raw/master/apple.china.conf' >> china_domain_list.tmp

  # get domains
  sed -i 's/server=\//    "/g' china_domain_list.tmp
  sed -i 's/\/114.114.114.114/": 1,/g' china_domain_list.tmp

  # exclude comment lines
  sed -i '/#/d' china_domain_list.tmp

  # remove last ',' character via: https://stackoverflow.com/questions/3576139/sed-remove-string-only-in-the-last-line-of-the-file
  sed -i '$ s/": 1,/": 1/g' china_domain_list.tmp

  sed 's/__CHINA_DOMAINS_PLACEHOLDER__/cat china_domain_list.tmp/e' ../../template/whitelist.pac > whitelist.pac

  sed -i "1i//\n// Update: $(date +'%Y-%m-%d %T')\n//\n" whitelist.pac
  popd
}

# gfw blacklist pac
function gen_gfwlist_pac() {
  mkdir -p gen/pac
  pushd gen/pac

  curl -kL 'https://github.com/gfwlist/gfwlist/raw/master/gfwlist.txt' | base64 -d > gfwlist.txt

  # patterns from @cokebar/gfwlist2dnsmasq
  IGNORE_PATTERN='^\!|\[|^@@|(https?://){0,1}[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+'
  HEAD_FILTER_PATTERN='s#^(\|\|?)?(https?://)?##g'
  TAIL_FILTER_PATTERN='s#/.*$|%2F.*$##g'
  DOMAIN_PATTERN='([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)'
  WILDCARD_PATTERN='s#^(([a-zA-Z0-9]*\*[-a-zA-Z0-9]*)?(\.))?([a-zA-Z0-9][-a-zA-Z0-9]*(\.[a-zA-Z0-9][-a-zA-Z0-9]*)+)(\*)?#\4#g'

  grep -vE $IGNORE_PATTERN gfwlist.txt | sed -r $HEAD_FILTER_PATTERN | sed -r $TAIL_FILTER_PATTERN | grep -E $DOMAIN_PATTERN | sed -r $WILDCARD_PATTERN > gfwlist.tmp
  printf 'google.com\ngoogle.ad\ngoogle.ae\ngoogle.com.af\ngoogle.com.ag\ngoogle.com.ai\ngoogle.al\ngoogle.am\ngoogle.co.ao\ngoogle.com.ar\ngoogle.as\ngoogle.at\ngoogle.com.au\ngoogle.az\ngoogle.ba\ngoogle.com.bd\ngoogle.be\ngoogle.bf\ngoogle.bg\ngoogle.com.bh\ngoogle.bi\ngoogle.bj\ngoogle.com.bn\ngoogle.com.bo\ngoogle.com.br\ngoogle.bs\ngoogle.bt\ngoogle.co.bw\ngoogle.by\ngoogle.com.bz\ngoogle.ca\ngoogle.cd\ngoogle.cf\ngoogle.cg\ngoogle.ch\ngoogle.ci\ngoogle.co.ck\ngoogle.cl\ngoogle.cm\ngoogle.cn\ngoogle.com.co\ngoogle.co.cr\ngoogle.com.cu\ngoogle.cv\ngoogle.com.cy\ngoogle.cz\ngoogle.de\ngoogle.dj\ngoogle.dk\ngoogle.dm\ngoogle.com.do\ngoogle.dz\ngoogle.com.ec\ngoogle.ee\ngoogle.com.eg\ngoogle.es\ngoogle.com.et\ngoogle.fi\ngoogle.com.fj\ngoogle.fm\ngoogle.fr\ngoogle.ga\ngoogle.ge\ngoogle.gg\ngoogle.com.gh\ngoogle.com.gi\ngoogle.gl\ngoogle.gm\ngoogle.gp\ngoogle.gr\ngoogle.com.gt\ngoogle.gy\ngoogle.com.hk\ngoogle.hn\ngoogle.hr\ngoogle.ht\ngoogle.hu\ngoogle.co.id\ngoogle.ie\ngoogle.co.il\ngoogle.im\ngoogle.co.in\ngoogle.iq\ngoogle.is\ngoogle.it\ngoogle.je\ngoogle.com.jm\ngoogle.jo\ngoogle.co.jp\ngoogle.co.ke\ngoogle.com.kh\ngoogle.ki\ngoogle.kg\ngoogle.co.kr\ngoogle.com.kw\ngoogle.kz\ngoogle.la\ngoogle.com.lb\ngoogle.li\ngoogle.lk\ngoogle.co.ls\ngoogle.lt\ngoogle.lu\ngoogle.lv\ngoogle.com.ly\ngoogle.co.ma\ngoogle.md\ngoogle.me\ngoogle.mg\ngoogle.mk\ngoogle.ml\ngoogle.com.mm\ngoogle.mn\ngoogle.ms\ngoogle.com.mt\ngoogle.mu\ngoogle.mv\ngoogle.mw\ngoogle.com.mx\ngoogle.com.my\ngoogle.co.mz\ngoogle.com.na\ngoogle.com.nf\ngoogle.com.ng\ngoogle.com.ni\ngoogle.ne\ngoogle.nl\ngoogle.no\ngoogle.com.np\ngoogle.nr\ngoogle.nu\ngoogle.co.nz\ngoogle.com.om\ngoogle.com.pa\ngoogle.com.pe\ngoogle.com.pg\ngoogle.com.ph\ngoogle.com.pk\ngoogle.pl\ngoogle.pn\ngoogle.com.pr\ngoogle.ps\ngoogle.pt\ngoogle.com.py\ngoogle.com.qa\ngoogle.ro\ngoogle.ru\ngoogle.rw\ngoogle.com.sa\ngoogle.com.sb\ngoogle.sc\ngoogle.se\ngoogle.com.sg\ngoogle.sh\ngoogle.si\ngoogle.sk\ngoogle.com.sl\ngoogle.sn\ngoogle.so\ngoogle.sm\ngoogle.sr\ngoogle.st\ngoogle.com.sv\ngoogle.td\ngoogle.tg\ngoogle.co.th\ngoogle.com.tj\ngoogle.tk\ngoogle.tl\ngoogle.tm\ngoogle.tn\ngoogle.to\ngoogle.com.tr\ngoogle.tt\ngoogle.com.tw\ngoogle.co.tz\ngoogle.com.ua\ngoogle.co.ug\ngoogle.co.uk\ngoogle.com.uy\ngoogle.co.uz\ngoogle.com.vc\ngoogle.co.ve\ngoogle.vg\ngoogle.co.vi\ngoogle.com.vn\ngoogle.vu\ngoogle.ws\ngoogle.rs\ngoogle.co.za\ngoogle.co.zm\ngoogle.co.zw\ngoogle.cat\n' >> gfwlist.tmp
  printf 'blogspot.ca\nblogspot.co.uk\nblogspot.com\nblogspot.com.ar\nblogspot.com.au\nblogspot.com.br\nblogspot.com.by\nblogspot.com.co\nblogspot.com.cy\nblogspot.com.ee\nblogspot.com.eg\nblogspot.com.es\nblogspot.com.mt\nblogspot.com.ng\nblogspot.com.tr\nblogspot.com.uy\nblogspot.de\nblogspot.gr\nblogspot.in\nblogspot.mx\nblogspot.ch\nblogspot.fr\nblogspot.ie\nblogspot.it\nblogspot.pt\nblogspot.ro\nblogspot.sg\nblogspot.be\nblogspot.no\nblogspot.se\nblogspot.jp\nblogspot.in\nblogspot.ae\nblogspot.al\nblogspot.am\nblogspot.ba\nblogspot.bg\nblogspot.ch\nblogspot.cl\nblogspot.cz\nblogspot.dk\nblogspot.fi\nblogspot.gr\nblogspot.hk\nblogspot.hr\nblogspot.hu\nblogspot.ie\nblogspot.is\nblogspot.kr\nblogspot.li\nblogspot.lt\nblogspot.lu\nblogspot.md\nblogspot.mk\nblogspot.my\nblogspot.nl\nblogspot.no\nblogspot.pe\nblogspot.qa\nblogspot.ro\nblogspot.ru\nblogspot.se\nblogspot.sg\nblogspot.si\nblogspot.sk\nblogspot.sn\nblogspot.tw\nblogspot.ug\nblogspot.cat\n' >> gfwlist.tmp
  printf 'twimg.edgesuite.net\n' >> gfwlist.tmp
  cat gfwlist.tmp | sort | uniq > gfw_domain_list.tmp

  sed -i 's/^/    "/' gfw_domain_list.tmp
  sed -i 's/$/": 1,/' gfw_domain_list.tmp
  sed -i '$ s/": 1,/": 1/g' gfw_domain_list.tmp

  sed 's/__GFWLIST_DOMAINS_PLACEHOLDER__/cat gfw_domain_list.tmp/e' ../../template/gfwlist.pac > gfwlist.pac

  sed -i "1i//\n// Update: $(date +'%Y-%m-%d %T')\n//\n" gfwlist.pac
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

  rm gen/pac/gfwlist.txt
  rm gen/pac/gfwlist.tmp
  rm gen/pac/gfw_domain_list.tmp
}

function dist_release() {
  cp -r gen dist
}

gen_chnroute
gen_dnsmasq_rules
gen_whitelist_pac
gen_gfwlist_pac

clean_up
dist_release
