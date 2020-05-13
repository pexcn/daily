ODEF_UTIL_MISC  =
ODEF_UTIL_MISC += util/strutil
ODEF_UTIL_MISC += util/readfile
ODEF_UTIL_MISC += util/dynarray

ODEF_UTIL_PARSE_IP  =
ODEF_UTIL_PARSE_IP += util/parse_ip/common
ODEF_UTIL_PARSE_IP += util/parse_ip/ip4
ODEF_UTIL_PARSE_IP += util/parse_ip/ip6
ODEF_UTIL_PARSE_IP += util/parse_ip/mixed
ODEF_UTIL_PARSE_IP += util/parse_ip/combined
ODEF_UTIL_PARSE_IP += util/parse_ip/parsefile

ODEF_IP  =
#ODEF_IP += ip/base  # hdr only
ODEF_IP += ip/ip4/base
ODEF_IP += ip/ip4/addr
ODEF_IP += ip/ip6/base
ODEF_IP += ip/ip6/addr
#ODEF_IP += ip/addr  # hdr only

ODEF_IP_TREE  =
ODEF_IP_TREE += ip_tree/data/node
ODEF_IP_TREE += ip_tree/data/typedesc
ODEF_IP_TREE += ip_tree/data/tree
ODEF_IP_TREE += ip_tree/op/insert
ODEF_IP_TREE += ip_tree/op/collapse
ODEF_IP_TREE += ip_tree/op/collect_ip4
ODEF_IP_TREE += ip_tree/op/collect_ip6
ODEF_IP_TREE += ip_tree/op/invert
ODEF_IP_TREE += ip_tree/op/find
ODEF_IP_TREE += ip_tree/op/purge

ODEF_IP_TREE_IO  =
ODEF_IP_TREE_IO += ip_tree_io/builder
ODEF_IP_TREE_IO += ip_tree_io/print

ODEF_APP_IP_DEDUP  =
ODEF_APP_IP_DEDUP += app/ip-dedup/globals
ODEF_APP_IP_DEDUP += app/ip-dedup/main


# ---

OBUNDLE_IP  =
OBUNDLE_IP += $(ODEF_IP)
OBUNDLE_IP += $(ODEF_UTIL_PARSE_IP)

OBUNDLE_IP_TREE  =
OBUNDLE_IP_TREE += $(ODEF_UTIL_MISC)
OBUNDLE_IP_TREE += $(OBUNDLE_IP)
OBUNDLE_IP_TREE += $(ODEF_IP_TREE)
OBUNDLE_IP_TREE += $(ODEF_IP_TREE_IO)

OBUNDLE_APP_IP_DEDUP  =
OBUNDLE_APP_IP_DEDUP += $(OBUNDLE_IP_TREE)
OBUNDLE_APP_IP_DEDUP += $(ODEF_APP_IP_DEDUP)
