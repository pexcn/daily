#!/bin/sh

TOPLIST_URL="https://s3-us-west-1.amazonaws.com/umbrella-static/top-1m.csv.zip"

curl -L $TOPLIST_URL |
  gunzip |
  # extract domain
  cut -d',' -f2 |
  # convert to unix format
  sed 's/\r$//'
