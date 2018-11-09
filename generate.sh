#!/bin/bash -e

bash scripts/adblock/adblock.sh

bash scripts/chnroute/chnroute.sh

bash scripts/gfwlist/gfwlist.sh

bash scripts/chinalist/chinalist.sh

bash scripts/pac/gfwlist.sh
bash scripts/pac/whitelist.sh

bash scripts/shadowrocket/gfwlist.sh
bash scripts/shadowrocket/tinylist.sh
bash scripts/shadowrocket/whitelist.sh
