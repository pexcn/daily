#!/bin/bash -e

# advertisement list
bash scripts/adlist/adlist.sh

# gfw domain list
bash scripts/gfwlist/gfwlist.sh

# china domain list
bash scripts/chinalist/chinalist.sh

# alexa top list
bash scripts/alexa/alexa.sh

# ------------------------------

# adblock
bash scripts/adblock/adblock.sh

# chnroute
bash scripts/chnroute/chnroute.sh

# safelist
bash scripts/safelist/safelist.sh

# pac
bash scripts/pac/gfwlist.sh
bash scripts/pac/whitelist.sh

# shadowrocket
bash scripts/shadowrocket/gfwlist.sh
bash scripts/shadowrocket/whitelist.sh
