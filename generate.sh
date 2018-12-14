#!/bin/bash -e

# adlist
bash scripts/adlist/adlist.sh

# adblock
bash scripts/adblock/adblock.sh

# chnroute
bash scripts/chnroute/chnroute.sh

# alexa top list
bash scripts/alexa/alexa.sh

# gfw domain list
bash scripts/gfwlist/gfwlist.sh

# china domain list
bash scripts/chinalist/chinalist.sh

# pac
bash scripts/pac/gfwlist.sh
bash scripts/pac/whitelist.sh

# shadowrocket
bash scripts/shadowrocket/gfwlist.sh
bash scripts/shadowrocket/whitelist.sh
