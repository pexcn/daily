# netaggregate

netaggregate is a tool to aggregate/summarize a list of IPv6 or IPv4 networks, addresses, CIDRs, prefixes.
It expects IPv6 or IPv4 networks or address/length combination on stdint and outputs results of the aggregation to stdout.
One can mix IPv6 and IPv4.
IPs without mask are considered having prefix length of /128 and /32 respectively.
If address/length is supplied, address part is truncated to the correct network prefix, i.e. bits are zeroed to match the length.
Example of the input:
```
2001:DB8::/120
10.10.0.1/16
2001:dB8::123
172.16.14.14
2001:Db8::FfFf/96

```
Corresponding output:
```
10.10.0.0/16
172.16.14.14/32
2001:db8::/96
```
Usage examples:
```
netaggregate < test-in.txt > result-out.txt
```
```
echo -e '2001:DB8::/120\n2001:DB8::FfFf/96' | netaggregate
```
