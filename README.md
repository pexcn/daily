# 📅 Daily

[![CI](https://github.com/pexcn/daily/workflows/CI/badge.svg)](https://github.com/pexcn/daily/actions)
[![CDN](https://data.jsdelivr.com/v1/package/gh/pexcn/daily/badge/day)](https://cdn.jsdelivr.net/gh/pexcn/daily@gh-pages/)

This script can be automatically generate something what you needed every day.

[![Flag Counter](https://s04.flagcounter.com/countxl/jtlp/bg_FFFFFF/txt_000000/border_CCCCCC/columns_2/maxflags_10/viewers_3/labels_1/pageviews_1/flags_0/percent_0/)](https://info.flagcounter.com/jtlp)

[![Powered by DartNode](https://github.com/bin456789/reinstall/assets/7548515/435d6740-bcdd-4f3a-a196-2f60ae397f17)](https://dartnode.com "Powered by DartNode - Free VPS for Open Source")
[![Powered by DartNode](https://dartnode.com/branding/DN-Open-Source-sm.png)](https://dartnode.com "Powered by DartNode - Free VPS for Open Source")

## 🍭 Lists

- [`adlist/adlist.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/adlist/adlist.txt)
- [`adblock/adblock.conf`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/adblock/adblock.conf)
- [`chnroute/chnroute.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/chnroute/chnroute.txt)
- [`chnroute/chnroute6.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/chnroute/chnroute6.txt)
- [`chinalist/chinalist.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/chinalist/chinalist.txt)
- [`gfwlist/gfwlist.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/gfwlist/gfwlist.txt)
- [`pac/gfwlist.pac`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/pac/gfwlist.pac)
- [`pac/whitelist.pac`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/pac/whitelist.pac)
- [`shadowrocket/gfwlist.conf`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/shadowrocket/gfwlist.conf)
- [`shadowrocket/whitelist.conf`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/shadowrocket/whitelist.conf)
- [`trackerlist/trackerlist.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/trackerlist/trackerlist.txt)
- [`trackerlist/trackerlist-best.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/trackerlist/trackerlist-best.txt)
- [`trackerlist/trackerlist-aria2.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/trackerlist/trackerlist-aria2.txt)
- [`trackerlist/trackerlist-best-aria2.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/trackerlist/trackerlist-best-aria2.txt)
- [`ublacklist/ublacklist.txt`](https://raw.githubusercontent.com/pexcn/daily/gh-pages/ublacklist/ublacklist.txt)

***Note: The [`pac`](https://github.com/pexcn/daily/tree/gh-pages/pac) list is only designed for [SwithcyOmega](https://github.com/FelisCatus/SwitchyOmega).***

### ~~💡 Tips~~

~~Visit via [jsDelivr CDN](https://cdn.jsdelivr.net/gh/pexcn/daily@gh-pages/).~~

### 📡 OpenWrt

1. Put the [`scripts`](openwrt) into OpenWrt.
2. Add the following cron jobs.

```bash
# adblock
0 3 * * * /path/to/adblock.sh

# chinadns-ng
1 3 * * * /path/to/chinadns-ng.sh

# chinadns
2 3 * * * /path/to/chinadns.sh

# transproxy
3 3 * * * /path/to/transproxy.sh
```

## 🙌 Credits

- [alexa-static](https://s3.amazonaws.com/alexa-static/top-1m.csv.zip)
- [apnic](https://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest)
- [dnsmasq-china-list](https://github.com/felixonmars/dnsmasq-china-list)
- [gfwlist](https://github.com/gfwlist/gfwlist)
- [trackerslist](https://github.com/ngosang/trackerslist)
- [ublacklist](https://github.com/pexcn/ublacklist)
- [ip-dedup](https://github.com/dywisor/ip-dedup)
- [netaggregate](https://github.com/afpd/netaggregate)
- [aggregate](https://ftp.isc.org/isc/aggregate/)
- [cidrmerge](http://cidrmerge.sourceforge.net)

## 📝 License

```
Copyright (C) 2018-2023, pexcn <pexcn97@gmail.com>

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
