#!/bin/bash -e

bash scripts/chnroute/chnroute.sh
bash scripts/adblock/adblock.sh
bash scripts/pac/whitelist.sh
bash scripts/pac/gfwlist.sh
bash scripts/shadowrocket/gfwlist.sh
