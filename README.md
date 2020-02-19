# Daily

[![Build Status](https://travis-ci.org/pexcn/daily.svg?branch=master)](https://travis-ci.org/pexcn/daily)

This script can be automatically generate something what I needed every day.

> Telegram Group: https://t.me/daily_scripts

## Usage

- [`adlist/adlist.txt`](https://pexcn.me/daily/adlist/adlist.txt)
- [`adblock/adblock.conf`](https://pexcn.me/daily/adblock/adblock.conf)
- [`chnroute/chnroute.txt`](https://pexcn.me/daily/chnroute/chnroute.txt)
- [`gfwlist/gfwlist.txt`](https://pexcn.me/daily/gfwlist/gfwlist.txt)
- [`chinalist/chinalist.txt`](https://pexcn.me/daily/chinalist/chinalist.txt)
- [`pac/gfwlist.pac`](https://pexcn.me/daily/pac/gfwlist.pac)
- [`pac/whitelist.pac`](https://pexcn.me/daily/pac/whitelist.pac)
- [`shadowrocket/gfwlist.conf`](https://pexcn.me/daily/shadowrocket/gfwlist.conf)
- [`shadowrocket/whitelist.conf`](https://pexcn.me/daily/shadowrocket/whitelist.conf)

Note: The PAC list is only designed for [SwithcyOmega](https://github.com/FelisCatus/SwitchyOmega).

### OpenWRT

1. Save your needed [`scripts`](openwrt) into OpenWRT.
2. Add the following cron jobs.

```bash
# adblock
0 3 * * * </path/to/adblock_script>

# chinadns-ng
1 3 * * * </path/to/chinadns-ng_script>

# chinadns
2 3 * * * </path/to/chinadns_script>
```

## Credits

- [@shadowsocks/ChinaDNS](https://github.com/shadowsocks/ChinaDNS)
- [@felixonmars/dnsmasq-china-list](https://github.com/felixonmars/dnsmasq-china-list)
- [@clowwindy/gfwlist2pac](https://github.com/clowwindy/gfwlist2pac)
- [@cokebar/gfwlist2dnsmasq](https://github.com/cokebar/gfwlist2dnsmasq)
- [@rssnsj/network-feeds](https://github.com/rssnsj/network-feeds)
- [@R0uter/gfw_domain_whitelist](https://github.com/R0uter/gfw_domain_whitelist)
- [@breakwa11/gfw_whitelist](https://github.com/breakwa11/gfw_whitelist)
- [@breakwa11/GFWList2PAC](https://github.com/breakwa11/GFWList2PAC)
- [@Leask/Flora_Pac](https://github.com/Leask/Flora_Pac)
- [aggregate](https://ftp.isc.org/isc/aggregate/)
- [cidrmerge](http://cidrmerge.sourceforge.net)

## License

```
Copyright (C) 2018 - 2020, pexcn <i@pexcn.me>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```
