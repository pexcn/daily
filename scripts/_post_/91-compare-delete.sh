#!/bin/bash -e
set -o pipefail

CUR_DIR=$(pwd)
TMP_DIR=$(mktemp -d /tmp/compare.XXXXXX)

compare_and_delete() {
  cd $TMP_DIR

  local dist_file=$CUR_DIR/dist/$1
  local release_file=$CUR_DIR/release/$1
  local skip_offset=$(($2+1))

  tail -n +$skip_offset $dist_file > dist.cut
  tail -n +$skip_offset $release_file > release.cut

  cmp -s dist.cut release.cut && rm $dist_file

  cd $CUR_DIR
}

clean_up() {
  rm -r $TMP_DIR
  echo "[compare]: finished."
}

compare_and_delete adblock/adblock.conf 3
compare_and_delete pac/gfwlist.pac 3
compare_and_delete pac/whitelist.pac 3
compare_and_delete shadowrocket/gfwlist.conf 3
compare_and_delete shadowrocket/whitelist.conf 3
clean_up
