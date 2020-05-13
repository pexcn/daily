CC_OPTS_EXTRA += -fstack-protector-strong
CC_OPTS_EXTRA += -D_FORTIFY_SOURCE=2
CC_OPTS_EXTRA += -fpie
LDFLAGS_EXTRA += -Wl,-pie
#CC_OPTS_EXTRA += -fstack-clash-protection
LDFLAGS_EXTRA += -Wl,-z,defs
LDFLAGS_EXTRA += -Wl,-z,now
LDFLAGS_EXTRA += -Wl,-z,relro
