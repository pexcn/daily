#!/bin/sh -e

TARGET_URL="https://pexcn.me/daily/chnroute/chnroute.txt"
TARGET_PATH="/etc/cleandns_chnroute.txt"
TEMP_FILE="/tmp/$(date +%s).tmp"

wget $TARGET_URL -O $TEMP_FILE -T 10 --no-cookies --no-hsts
mkdir -p $(dirname $TARGET_PATH) && mv -f $TEMP_FILE $TARGET_PATH
/etc/init.d/cleandns restart
/etc/init.d/shadowsocks restart
