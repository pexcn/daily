#!/bin/sh -e

[ -d /etc/dnsmasq.d ] || mkdir /etc/dnsmasq.d
wget https://raw.githubusercontent.com/pexcn/daily/gh-pages/adblock/adblock.conf -O /tmp/adblock.conf.tmp
mv -f /tmp/adblock.conf.tmp /etc/dnsmasq.d/adblock.conf

/etc/init.d/dnsmasq restart
