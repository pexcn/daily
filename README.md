# Daily

[![Build Status](https://travis-ci.org/pexcn/daily.svg?branch=master)](https://travis-ci.org/pexcn/daily)

This script can be automatically generate something every day.

## Usage

* `chnroute.txt` is the routing table for ChinaDNS.
* `adblock.conf` is dnsmasq advertising blocking rules.

### OpenWRT

Add script in `/root/update/chinadns.sh`

```bash
#!/bin/ash -e

wget -O /tmp/chnroute.txt https://pexcn.github.io/daily/chnroute/chnroute.txt --no-check-certificate
mv /tmp/chnroute.txt /etc/chinadns_chnroute.txt
/etc/init.d/chinadns restart
```

Add script in `/root/update/adblock.sh`

```bash
#!/bin/ash -e

wget -O /tmp/adblock.conf https://pexcn.github.io/daily/dnsmasq/adblock.conf --no-check-certificate
mv /tmp/adblock.conf /etc/dnsmasq.d/adblock.conf
/etc/init.d/dnsmasq restart
```

So you can edit crontab to set cron jobs, just like:

```bash
# Update chnroute
0 3 * * * /root/update/chinadns.sh

# Update adblock rules
1 3 * * * /root/update/adblock.sh
```

## License

```
Copyright (C) 2018, pexcn <pexcn97@gmail.com>

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
