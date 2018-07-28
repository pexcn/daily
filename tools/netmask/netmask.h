
#include <netinet/in.h>
#include <netdb.h>

typedef struct nm *NM;

NM nm_new_v4(struct in_addr *);

NM nm_new_v6(struct in6_addr *);

NM nm_new_ai(struct addrinfo *);

#define NM_USE_DNS 1

NM nm_new_str(const char *, int flags);

NM nm_merge(NM, NM);

typedef union {
    struct in6_addr s6;
    struct in_addr  s;
} nm_addr;

void nm_walk(NM, void (*)(int domain, nm_addr *neta, nm_addr *mask));

