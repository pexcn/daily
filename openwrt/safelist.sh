#!/bin/sh -e

TARGET_URL="https://pexcn.me/daily/safelist/safelist.conf"
TARGET_PATH="/etc/dnsmasq.d/safelist.conf"
TEMP_FILE="/tmp/$(date +%s).tmp"

wget $TARGET_URL -O $TEMP_FILE -T 10 --no-cookies --no-hsts
mkdir -p $(dirname $TARGET_PATH) && mv -f $TEMP_FILE $TARGET_PATH
/etc/init.d/dnsmasq restart
