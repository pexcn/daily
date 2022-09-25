#!/bin/sh -e

[ -d /etc/transproxy ] || mkdir /etc/transproxy

wget https://raw.githubusercontent.com/pexcn/daily/gh-pages/chnroute/chnroute.txt -O /etc/transproxy/chnroute.tmp
mv -f /etc/transproxy/chnroute.tmp /etc/transproxy/chnroute.txt

wget https://raw.githubusercontent.com/pexcn/daily/gh-pages/chnroute/chnroute6.txt -O /etc/transproxy/chnroute6.tmp
mv -f /etc/transproxy/chnroute6.tmp /etc/transproxy/chnroute6.txt

/etc/init.d/transproxy restart
