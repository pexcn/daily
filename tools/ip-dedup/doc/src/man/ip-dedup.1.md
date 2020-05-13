% IP-DEDUP(1) Version @VERSION@ | ip-dedup

NAME
====

**ip-dedup** — deduplicate/aggregate IPv4/IPv6 addresses/networks


SYNOPSIS
========

| **ip-dedup** \[**-4**|**-6**|**-a**] \[**-B** _n_] \[**-C** _n_|**-c**] \[**-D** _dir_] \[**-i**\] \[**-k**\] \[**-l**\]
|          \[**-o** _file_] \[**-p** _file_] \[**-s**\] \[_file_...]
| **ip-dedup** **-L**
| **ip-dedup** **-h**



DESCRIPTION
===========

Merges and aggregates IPv4/IPv6 addresses/networks and writes
the resulting networks in <addr>/<prefixlen> notation to stdout.
Networks can be excluded from the final result by using the purge option.

The output will be sorted. When operating in mixed mode,
IPv4 networks will be written before any IPv6 networks.

Input networks are read from stdin or the given files.
The input format is one network address per line,
optionally followed by a slash char '/' and a prefixlen.
If omitted, the prefixlen is guessed based on the number
of non-zero blocks in the network address.

Input and exclude file names starting with a '@' char are looked up
in the data directory after removing the prefix char.

For each line in the input files,
leading and trailing whitespace is removed.
Empty lines, possibly caused by this conversion, are silently ignored.
Likewise, lines starting with a '#' char are ignored, too.

The keep-going mode (-k) allows for invalid input.
However, if no input network specification could be parsed
but at least one was seen, then ip-dedup exits non-zero (65).
This rule applies to each individual input file.


Options
-------

-4

:   _IPv4-mode_ - networks get parsed as IPv4 addresses

-6

:   _IPv6-mode_ - networks get parsed as IPv6 addresses

-a

:   (automatic) _mixed-mode_ - all networks get parsed as both IPv4 and IPv6 addresses on a best-effort basis

    This is the default mode if neither **-4** nor **-6** are specified.

    At least one parse attempt must succeed, otherwise the input is considered to be invalid.
    If a network can be parsed as both IPv4 and IPv6 address (e.g. 0/0),
    then both addresses will be used.

-B _n_

:   truncate IPv4 networks if they exceed the given prefix length

-C _n_

:   truncate IPv6 networks if they exceed the given prefix length

-c

:   truncate IPv6 networks with a prefixlen greater than 64 to /64 network addresses (same as **-C** 64)

-D _dir_

:   look up all following _datadir files_ in the given directory

-h

:   print the help message and exit

-i

:   invert so that all networks except the input ones appear in the output

    The resulting network addresses are subject to purging (**-p**).

-k

:   keep going - skip invalid lines in input instead of exiting

-L

:   list include files found in the default _datadir_

-l

:   enable long output format:

    - IPv6 addresses will be written in their full-length form (_IPv6-mode_ only)

-o _file_

:   write output to a file instead of stdout

-p _file_

:   purge - exclude certain networks from appearing in the output

    The excludes will be read from the given file.
    This option may be specified more than once.

-s

:   strict - exit non-zero if host bits are set in parsed addresses

    By default, network addresses get silently truncated to their prefix length.


EXIT CODES
==========

0

:   success

1

:   unspecified error

64

:   usage error

65

:   failed to parse an input or purge network specification

70

:   software logic broken

71

:   failed to get system resources

73

:   failed to open output file

74

:   failed to open input file


FILES
=====

*@DATADIR@*

:   input/purge files starting with a '@' character are looked
    up relative to this directory after removing the '@'.


BUGS
====

See GitHub Issues: <https://github.com/dywisor/ip-dedup/issues>


AUTHOR
======

André Erdmann <dywi@mailerd.de>
