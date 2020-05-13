#!/bin/bash

pre() {
  local count=$(ls -1 scripts/pre/*.sh 2>/dev/null | wc -l)
  if [ $count == 0 ]; then
    return
  fi

  for file in scripts/pre/*.sh
  do
    "$file"
  done
}

post() {
  local count=$(ls -1 scripts/post/*.sh 2>/dev/null | wc -l)
  if [ $count == 0 ]; then
    return
  fi

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
