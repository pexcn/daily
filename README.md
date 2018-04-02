# Daily build

[![Build Status](https://travis-ci.org/pexcn/daily.svg?branch=master)](https://travis-ci.org/pexcn/daily)

## How to use?

See [https://github.com/pexcn/MyNote/tree/master/Notes/OpenWrt](https://github.com/pexcn/MyNote/tree/master/Notes/OpenWrt).

## TODO

```bash
# Full
# 注意这个 ad_server.txt 行尾是 DOS, 这里使用 sed 替换为 Unix
wget -4 --no-check-certificate -O - https://hosts-file.net/ad_servers.txt | sed  $'s/\r$//' | grep -E '^127.0.0.1' | awk '{printf "address=/%s/127.0.0.1\n",$2}' | sort > /tmp/adaway_max.conf

# Lite
wget -4 --no-check-certificate -O - https://adaway.org/hosts.txt | grep -E '^127.0.0.1' | awk '{printf "address=/%s/127.0.0.1\n",$2}' | sort > /tmp/adaway_mini.conf
```
