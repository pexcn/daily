#!/bin/sh -e

wget -O /tmp/safelist.conf https://pexcn.me/daily/safelist/safelist.conf
mv /tmp/safelist.conf /etc/dnsmasq.d/safelist.conf
/etc/init.d/dnsmasq restart
