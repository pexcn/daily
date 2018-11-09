#!/bin/bash -e

bash scripts/chnroute/chnroute.sh

bash scripts/adblock/adblock.sh

bash scripts/pac/gfwlist.sh
bash scripts/pac/whitelist.sh

bash scripts/shadowrocket/gfwlist.sh
bash scripts/shadowrocket/tinylist.sh
bash scripts/shadowrocket/whitelist.sh
