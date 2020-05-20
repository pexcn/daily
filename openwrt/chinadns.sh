#!/bin/sh -e
set -o pipefail

wget https://pexcn.me/daily/chnroute/chnroute.txt -O /tmp/chnroute.txt.tmp
mv -f /tmp/chnroute.txt.tmp /etc/chinadns_chnroute.txt

/etc/init.d/chinadns restart
/etc/init.d/shadowsocks restart
