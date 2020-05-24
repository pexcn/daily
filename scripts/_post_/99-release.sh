#!/bin/bash -e
set -o pipefail

#
# diff_delete <dist_file> <release_file> <skip_offset>
#
diff_delete() {
  local dist_file="$1"
  local release_file="$2"
  local skip_offset=$(("$3"+1))
  cmp -s <(tail -n +$skip_offset $dist_file) <(tail -n +$skip_offset $release_file) && rm $dist_file
}

clone_release() {
  git clone https://github.com/pexcn/daily.git release -b gh-pages --depth 1 --single-branch --quiet
  rm -rf release/.git
}

clean_dist() {
  diff_delete dist/adblock/adblock.conf release/adblock/adblock.conf 4
  diff_delete dist/pac/gfwlist.pac release/pac/gfwlist.pac 4
  diff_delete dist/pac/whitelist.pac release/pac/whitelist.pac 4
  diff_delete dist/shadowrocket/gfwlist.conf release/shadowrocket/gfwlist.conf 4
  diff_delete dist/shadowrocket/whitelist.conf release/shadowrocket/whitelist.conf 4
}

copy_dist() {
  # avoid losing files when scripts execute failed
  cp -rf dist/* release
}

clone_release
clean_dist
copy_dist
