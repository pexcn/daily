#!/bin/bash -xe

function setup_env() {
  export TZ=Asia/Shanghai
  git config --global user.name "Travis CI"
  git config --global user.email "travis.ci.build@gmail.com"
  git config --global log.date iso
}

function prepare_env() {
  # cidrmerge
  mkdir -p build
  pushd build
  curl -kL https://sourceforge.net/projects/cidrmerge/files/latest/download -o cidrmerge.tar.gz
  tar zxvf cidrmerge.tar.gz && rm cidrmerge.tar.gz
  pushd cidrmerge
  make
  popd
  popd
}

function build_chnroute() {
  # ipv4 chnroute
  mkdir -p build/chnroute
  pushd build/chnroute
  curl -kL 'http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest' | grep ipv4 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > chnroute.txt.tmp
  cat chnroute.txt.tmp | ../cidrmerge/cidrmerge > chnroute.txt && rm chnroute.txt.tmp
  popd

  # ipv6 chnroute
  mkdir -p build/chnroute
  pushd build/chnroute
  curl -kL 'http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest' | grep ipv6 | grep CN | awk -F\| '{ printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > chnroute_v6.txt.tmp
  mv chnroute_v6.txt.tmp chnroute_v6.txt
  popd
}

# TODO: write it pretty
function build_dnsmasq_rules() {
  # dnsmasq rules
  mkdir -p build/dnsmasq
  pushd build/dnsmasq
  echo -e "#\n# easylistchina+easylist\n#" >> adblock.conf.tmp
  curl -kL https://easylist-downloads.adblockplus.org/easylistchina+easylist.txt | grep ^\|\|[^\*]*\^$ | sed -e 's:||:address\=\/:' -e 's:\^:/127\.0\.0\.1:' >> adblock.conf.tmp
  echo -e "\n\n" >> adblock.conf.tmp
  echo -e "#\n# ABP-FX\n#" >> adblock.conf.tmp
  curl -kL https://raw.githubusercontent.com/xinggsf/Adblock-Plus-Rule/master/ABP-FX.txt | grep ^\|\|[^\*]*\^$ | sed -e 's:||:address\=\/:' -e 's:\^:/127\.0\.0\.1:' >> adblock.conf.tmp
  echo -e "\n\n" >> adblock.conf.tmp
  echo -e "#\n# custom rules\n#" >> adblock.conf.tmp
  curl -kL https://pexcn.github.io/dnsmasq-rules/rules.conf >> adblock.conf.tmp
  mv adblock.conf.tmp adblock.conf
  popd
}

function clean_up() {
  # remove unused files
  pushd build
  rm -r cidrmerge
  popd
}

function release() {
  git clone https://github.com/pexcn/daily.git -b gh-pages release --depth 5
  pushd release
  rm -r *
  cp -r ../build/* .
  git add --all
  git commit -m "[AUTO BUILD] `date +'%Y-%m-%d %T'`" || echo "INFO: UP TO DATE"
  git push --quiet "https://${token}@github.com/pexcn/daily.git" gh-pages
  popd
}

setup_env
prepare_env

build_chnroute
build_dnsmasq_rules

clean_up
release
