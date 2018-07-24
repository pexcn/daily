# Daily

[![Build Status](https://travis-ci.org/pexcn/daily.svg?branch=master)](https://travis-ci.org/pexcn/daily)

This script can be automatically generate something what I needed every day.

## Usage

- [`chnroute.txt`](https://pexcn.me/daily/chnroute/chnroute.txt) is the routing table for ChinaDNS.
- [`adblock.conf`](https://pexcn.me/daily/adblock/adblock.conf) is dnsmasq ad blocking rules.
- [`gfwlist.pac`](https://pexcn.me/daily/pac/gfwlist.pac) is the PAC list of GFW domain blacklist.
- [`whitelist.pac`](https://pexcn.me/daily/pac/whitelist.pac) is the PAC list of GFW domain whitelist.

### OpenWRT

1. Save your needed `scripts` into OpenWRT.
2. Add the following cron jobs.

```bash
# update chnroute
0 3 * * * <path/to/chinadns_script>

# update adblock rules
1 3 * * * <path/to/adblock_script>
```

## Thanks

- [ChinaDNS](https://github.com/shadowsocks/ChinaDNS)
- [dnsmasq-china-list](https://github.com/felixonmars/dnsmasq-china-list)
- [gfwlist2pac](https://github.com/clowwindy/gfwlist2pac)
- [gfw_domain_whitelist](https://github.com/R0uter/gfw_domain_whitelist)
- [gfw_whitelist](https://github.com/breakwa11/gfw_whitelist)
- [GFWList2PAC](https://github.com/breakwa11/GFWList2PAC)
- [Flora_Pac](https://github.com/Leask/Flora_Pac)

## License

```
Copyright (C) 2018, pexcn <i@pexcn.me>

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
