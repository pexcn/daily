#!/bin/ash -e

wget -O /tmp/chnroute.txt https://pexcn.me/daily/chnroute/chnroute.txt
mv /tmp/chnroute.txt /etc/chinadns_chnroute.txt
/etc/init.d/chinadns restart
