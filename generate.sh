#!/bin/bash

pre() {
  local count=$(find scripts/_pre_ -maxdepth 1 -type f -iname "*.sh" | wc -l)
  [ $count == 0 ] && return

  for file in scripts/_pre_/*.sh
  do
    "$file"
  done
  echo "[pre]: done."
}

post() {
  local count=$(find scripts/_post_ -maxdepth 1 -type f -iname "*.sh" | wc -l)
  [ $count == 0 ] && return

  for file in scripts/_post_/*.sh
  do
    "$file"
  done
  echo "[post]: done."
}

main() {
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
  scripts/pac/pac.sh

  # shadowrocket
  scripts/shadowrocket/shadowrocket.sh

  # trackerlist
  scripts/trackerlist/trackerlist.sh

  # ublacklist
  scripts/ublacklist/ublacklist.sh
}

pre
main
post
