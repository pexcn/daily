#!/bin/sh -e
set -o pipefail

TARGET_URL_1="https://pexcn.me/daily/chnroute/chnroute.txt"
TARGET_PATH_1="/etc/chinadns-ng/chnroute.txt"
TARGET_URL_2="https://pexcn.me/daily/chnroute/chnroute-v6.txt"
TARGET_PATH_2="/etc/chinadns-ng/chnroute6.txt"
TARGET_URL_3="https://pexcn.me/daily/gfwlist/gfwlist.txt"
TARGET_PATH_3="/etc/chinadns-ng/gfwlist.txt"
TARGET_URL_4="https://pexcn.me/daily/chinalist/chinalist.txt"
TARGET_PATH_4="/etc/chinadns-ng/chinalist.txt"
TEMP_FILE="/tmp/$(date +%s).tmp"

wget $TARGET_URL_1 -O $TEMP_FILE -T 10 --no-cookies --no-hsts
mkdir -p $(dirname $TARGET_PATH_1) && mv -f $TEMP_FILE $TARGET_PATH_1
wget $TARGET_URL_2 -O $TEMP_FILE -T 10 --no-cookies --no-hsts
mkdir -p $(dirname $TARGET_PATH_2) && mv -f $TEMP_FILE $TARGET_PATH_2
wget $TARGET_URL_3 -O $TEMP_FILE -T 10 --no-cookies --no-hsts
mkdir -p $(dirname $TARGET_PATH_3) && mv -f $TEMP_FILE $TARGET_PATH_3
wget $TARGET_URL_4 -O $TEMP_FILE -T 10 --no-cookies --no-hsts
mkdir -p $(dirname $TARGET_PATH_4) && mv -f $TEMP_FILE $TARGET_PATH_4

/etc/init.d/chinadns-ng restart
/etc/init.d/shadowsocks restart
