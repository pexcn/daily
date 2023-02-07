#!/bin/sh -e

wget https://raw.githubusercontent.com/pexcn/daily/gh-pages/chnroute/chnroute.txt -O /tmp/chnroute.txt.tmp
mv -f /tmp/chnroute.txt.tmp /etc/chinadns_chnroute.txt

/etc/init.d/chinadns restart
