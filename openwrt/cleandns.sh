#!/bin/ash -e

wget -O /tmp/chnroute.txt https://pexcn.me/daily/chnroute/chnroute.txt
mv /tmp/chnroute.txt /etc/cleandns_chnroute.txt
/etc/init.d/cleandns restart
/etc/init.d/shadowsocks restart
