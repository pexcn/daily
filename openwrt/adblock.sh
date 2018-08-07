#!/bin/ash -e

wget -O /tmp/adblock.conf https://pexcn.me/daily/adblock/adblock.conf --no-check-certificate
mv /tmp/adblock.conf /etc/dnsmasq.d/adblock.conf
/etc/init.d/dnsmasq restart
