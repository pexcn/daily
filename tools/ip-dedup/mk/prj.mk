PN := ip-dedup

S_FILES  = $(S)/files
S_IPLIST = $(S_FILES)/list

# whether to build a binary that will be run from the git repo
STANDALONE ?= 0

# whether to disable -Werror
NO_WERROR ?= 0

# whether to build a static binary
STATIC ?= 0

# pass some hardening flags to the compiler
#  Note that HARDEN==1 is not compatible with STATIC==1
HARDEN ?= 1
