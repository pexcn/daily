#!/bin/bash

pre() {
  local count=$(find scripts/pre -maxdepth 1 -type f -iname "*.sh" | wc -l)
  [ $count == 0 ] && return

  for file in scripts/pre/*.sh
  do
    "$file"
  done
}

post() {
  local count=$(find scripts/post -maxdepth 1 -type f -iname "*.sh" | wc -l)
  [ $count == 0 ] && return

  for file in scripts/post/*.sh
  do
    "$file"
  done
}

run() {
  # ad domain list
  scripts/adlist/adlist.sh

  # china domain list
  scripts/chinalist/chinalist.sh

  # gfw domain list
  scripts/gfwlist/gfwlist.sh

  # ------------------------------

  # adblock
  scripts/adblock/adblock.sh

  # chnroute
  scripts/chnroute/chnroute.sh

  # pac
  scripts/pac/gfwlist.sh
  scripts/pac/whitelist.sh

  # shadowrocket
  scripts/shadowrocket/gfwlist.sh
  scripts/shadowrocket/whitelist.sh

  # ublacklist
  scripts/ublacklist/ublacklist.sh
}

pre
run
post
