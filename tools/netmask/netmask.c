/* netmask.c - a netmask generator
 *
 * Copyright (c) 2013  Robert Stone <talby@trap.mtview.ca.us>,
 *                     Tom Lear <tom@trap.mtview.ca.us>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include "errors.h"
#include "netmask.h"

typedef struct {
    uint64_t h;
    uint64_t l;
} u128_t;

static inline u128_t u128_add(u128_t x, u128_t y, int *carry) {
    /* this relies on the sum being greater than both terms of the
     * addition, otherwise an overflow must have occurred. */
    u128_t rv;
    rv.l = x.l + y.l;
    if(rv.l < x.l || rv.l < y.l)
        rv.h = 1;
    else
        rv.h = 0;
    rv.h += x.h + y.h;
    if(carry) {
        if(rv.h < x.h || rv.h < y.h)
            *carry = 1;
        else
            *carry = 0;
    }
    return rv;
}

static inline u128_t u128_and(u128_t x, u128_t y) {
    u128_t rv;
    rv.h = x.h & y.h;
    rv.l = x.l & y.l;
    return rv;
}

static inline u128_t u128_or(u128_t x, u128_t y) {
    u128_t rv;
    rv.h = x.h | y.h;
    rv.l = x.l | y.l;
    return rv;
}

static inline u128_t u128_xor(u128_t x, u128_t y) {
    u128_t rv;
    rv.h = x.h ^ y.h;
    rv.l = x.l ^ y.l;
    return rv;
}

static inline u128_t u128_neg(u128_t v) {
    u128_t rv;
    rv.h = ~v.h;
    rv.l = ~v.l;
    return rv;
}

static inline u128_t u128_lsh(u128_t v, uint8_t d) {
    u128_t rv;
    rv.h = v.h << 1 | v.l >> 63;
    rv.l = v.l << 1;
    return rv;
}

static inline int u128_cmp(u128_t x, u128_t y) {
    /* return -1, 0, 1 on sort order */
    if(x.h < y.h)
        return -1;
    if(x.h > y.h)
        return 1;
    if(x.l < y.l)
        return -1;
    if(x.l > y.l)
        return 1;
    return 0;
}

static inline u128_t u128_of_s6(struct in6_addr *s6) {
    u128_t rv;
    rv.h = (((uint64_t)s6->s6_addr[0])  << 56) |
           (((uint64_t)s6->s6_addr[1])  << 48) |
           (((uint64_t)s6->s6_addr[2])  << 40) |
           (((uint64_t)s6->s6_addr[3])  << 32) |
           (((uint64_t)s6->s6_addr[4])  << 24) |
           (((uint64_t)s6->s6_addr[5])  << 16) |
           (((uint64_t)s6->s6_addr[6])  <<  8) |
           (((uint64_t)s6->s6_addr[7])  <<  0);
    rv.l = (((uint64_t)s6->s6_addr[8])  << 56) |
           (((uint64_t)s6->s6_addr[9])  << 48) |
           (((uint64_t)s6->s6_addr[10]) << 40) |
           (((uint64_t)s6->s6_addr[11]) << 32) |
           (((uint64_t)s6->s6_addr[12]) << 24) |
           (((uint64_t)s6->s6_addr[13]) << 16) |
           (((uint64_t)s6->s6_addr[14]) <<  8) |
           (((uint64_t)s6->s6_addr[15]) <<  0);
    return rv;
}
static inline struct in6_addr s6_of_u128(u128_t v) {
    struct in6_addr s6;
    s6.s6_addr[0]  = 0xff & (v.h >> 56);
    s6.s6_addr[1]  = 0xff & (v.h >> 48);
    s6.s6_addr[2]  = 0xff & (v.h >> 40);
    s6.s6_addr[3]  = 0xff & (v.h >> 32);
    s6.s6_addr[4]  = 0xff & (v.h >> 24);
    s6.s6_addr[5]  = 0xff & (v.h >> 16);
    s6.s6_addr[6]  = 0xff & (v.h >>  8);
    s6.s6_addr[7]  = 0xff & (v.h >>  0);
    s6.s6_addr[8]  = 0xff & (v.l >> 56);
    s6.s6_addr[9]  = 0xff & (v.l >> 48);
    s6.s6_addr[10] = 0xff & (v.l >> 40);
    s6.s6_addr[11] = 0xff & (v.l >> 32);
    s6.s6_addr[12] = 0xff & (v.l >> 24);
    s6.s6_addr[13] = 0xff & (v.l >> 16);
    s6.s6_addr[14] = 0xff & (v.l >>  8);
    s6.s6_addr[15] = 0xff & (v.l >>  0);
    return s6;
}

static inline u128_t u128_lit(uint64_t h, uint64_t l) {
    u128_t rv;
    rv.h = h;
    rv.l = l;
    return rv;
}

static inline u128_t u128_cidr(uint8_t n) {
    u128_t rv;
    if(n <= 0) {
        rv.h = 0;
        rv.l = 0;
    } else if(n <= 64) {
        rv.h = ~0ULL << (64 - n);
        rv.l = 0;
    } else if(n <= 128) {
        rv.h = ~0ULL;
        rv.l = ~0ULL << (128 - n);
    } else {
        rv.h = ~0ULL;
        rv.l = ~0ULL;
    }
    return rv;
}
static inline int cidr(u128_t u) {
    uint64_t v;
    int n = 0;
    for(v = u.l; v > 0; v <<= 1) n++;
    for(v = u.h; v > 0; v <<= 1) n++;
    return n;
}

static inline int chkmask(u128_t v) {
    /* this is sort of specialized */
    int i;
    u128_t m = u128_lit(~0ULL, ~0ULL);

    for(i = 0; i < 129; i++) {
        if(u128_cmp(v, m) == 0)
            return 1;
        m = u128_lsh(m, 1);
    }
    return 0;
}

struct nm {
    u128_t neta;
    u128_t mask;
    int domain;
    NM next;
};

NM nm_new_v4(struct in_addr *s) {
    NM self;
    union {
      struct in6_addr s6;
      uint32_t u32[4];
    } v;

    v.u32[0] = 0;
    v.u32[1] = 0;
    v.u32[2] = htonl(0x0000ffff);
    v.u32[3] = s->s_addr;
    self = nm_new_v6(&v.s6);
    self->domain = AF_INET;
    return self;
}

NM nm_new_v6(struct in6_addr *s6) {
    NM self = (NM)malloc(sizeof(struct nm));
    self->neta = u128_of_s6(s6);
    self->mask = u128_cidr(128);
    self->domain = AF_INET6;
    self->next = (NM)0;
    return self;
}

/* is "a" a subset of "b"? */
static inline int subset_of(NM a, NM b) {
    return(
        u128_cmp(a->mask, b->mask) >= 0 &&
        u128_cmp(b->neta, u128_and(a->neta, b->mask)) == 0
    );
}
/* are "a" and "b" a joinable pair? */
static inline int joinable_pair(NM a, NM b) {
    return(
        /* nets have the same mask */
        u128_cmp(a->mask, b->mask) == 0 &&
        /* but are distinct */
        u128_cmp(a->neta, b->neta) != 0 &&
        /* and would both be subsets of the same mask << 1 */
        u128_cmp(u128_lit(0, 0), u128_and(
            u128_xor(a->neta, b->neta),
            u128_lsh(a->mask, 1)
        )) == 0
    );
}

/* this is slightly complicated because an NM can outgrow it's initial
 * v4 state, but if it doesn't, we want to retain the fact that it
 * was and remained v4.  */
static inline int is_v4(NM self) {
    struct nm v4map;

    v4map.neta = u128_lit(0, 0x0000ffff00000000ULL);
    v4map.mask = u128_cidr(96);

    return(self->domain == AF_INET && subset_of(self, &v4map));
}

NM nm_new_ai(struct addrinfo *ai) {
    NM self = NULL;
    struct addrinfo *cur;

    for(cur = ai; cur; cur = cur->ai_next) {
        switch(cur->ai_family) {
            case AF_INET:
                self = nm_merge(self, nm_new_v4(&(
                    (struct sockaddr_in *)cur->ai_addr
                )->sin_addr));
                break;
            case AF_INET6:
                self = nm_merge(self, nm_new_v6(&(
                    (struct sockaddr_in6 *)cur->ai_addr
                )->sin6_addr));
                break;
            default:
                panic("unknown ai_family %d in struct addrinfo",
                        cur->ai_family);
        }
    }
    return self;
}

static inline NM parse_addr(const char *str, int flags) {
    struct in6_addr s6;
    struct in_addr s;

    if(inet_pton(AF_INET6, str, &s6))
        return nm_new_v6(&s6);

    if(inet_aton(str, &s))
        return nm_new_v4(&s);

    if(NM_USE_DNS & flags) {
        struct addrinfo in, *out;

        memset(&in, 0, sizeof(struct addrinfo));
        in.ai_family = AF_UNSPEC;
        if(getaddrinfo(str, NULL, &in, &out) == 0) {
            NM self = nm_new_ai(out);
            freeaddrinfo(out);
            return self;
        }
    }
    return NULL;
}

static inline int parse_mask(NM self, const char *str, int flags) {
    char *p;
    uint32_t v;
    struct in6_addr s6;
    struct in_addr s;

    v = strtoul(str, &p, 0);
    if(*p == '\0') {
        /* read it as a CIDR value */
        if(is_v4(self)) {
            if(v < 0 || v > 32) return 0;
            v += 96;
        } else {
            if(v < 0 || v > 128) return 0;
        }
        self->mask = u128_cidr(v);
    } else if(inet_pton(AF_INET6, str, &s6)) {
        self->mask = u128_of_s6(&s6);
        /* flip cisco style masks */
        if(u128_cmp(
            u128_lit(0, 0),
            u128_and(
                u128_lit(1ULL << 63, 1),
                u128_xor(u128_lit(0, 1), self->mask)
            )
        ) == 0) {
            self->mask = u128_neg(self->mask);
        }
        self->domain = AF_INET6;
    } else if(self->domain == AF_INET && inet_aton(str, &s)) {
        v = htonl(s.s_addr);
        if(v & 1 && ~v >> 31) /* flip cisco style masks */
            v = ~v;
        /* since mask is currently all 1s, mask ^ ~m will
         * set the low 32. */
        self->mask = u128_xor(self->mask, u128_lit(0, ~v));
    } else {
        return 0;
    }
    if(!chkmask(self->mask))
        return 0;
    /* apply mask to neta */
    self->neta = u128_and(self->neta, self->mask);
    return 1;
}

/* widen the mask as much as possible without including addresses below
 * neta or above max.  return one if more ranges are needed to complete
 * the span or zero if this nm includes max. */
static inline int nm_widen(NM self, u128_t max, u128_t *last) {
    u128_t mask, neta, bcst;
    int cmp = u128_cmp(self->neta, max);

    while(cmp < 0) {
        /* attempt widening by one bit */
        mask = u128_lsh(self->mask, 1);
        neta = u128_and(self->neta, mask);
        bcst = u128_or(self->neta, u128_neg(mask));
        /* check ranges */
        if(u128_cmp(neta, self->neta) < 0)
            break;
        cmp = u128_cmp(bcst, max);
        if(cmp > 0)
            break;
        /* successful attempt */
        self->mask = mask;
        *last = bcst;
        status("widen %016llx %016llx/%d", self->neta.h, self->neta.l, cidr(self->mask));
        if(cmp == 0)
            break;
    }
    return cmp;
}

static inline void nm_order(NM *low, NM *high) {
    if(u128_cmp((*low)->neta, (*high)->neta) > 0) {
        NM tmp = *low;
        *low = *high;
        *high = tmp;
    }
}

/* convert first and last into a list from first to last.  (both these
 * should be single addresses, not lists.) */
static inline NM nm_seq(NM first, NM last) {
    /* if first is higher than last, swap them (legacy) */
    nm_order(&first, &last);
    NM cur = first;
    u128_t pos = cur->neta;
    u128_t one = u128_lit(0, 1);
    u128_t max = last->neta;
    int domain = is_v4(first) && is_v4(last) ? AF_INET : AF_INET6;

    free(last);
    while(nm_widen(cur, max, &pos)) {
        cur->next = (NM)malloc(sizeof(struct nm));
        cur = cur->next;
        cur->neta = u128_add(pos, one, NULL);
        cur->mask = u128_cidr(128);
        cur->domain = domain;
        cur->next = NULL;
    }
    return first;
}

NM nm_new_str(const char *str, int flags) {
    char *p, buf[2048];
    NM self;

    if((p = strchr(str, '/'))) { /* mask separator */
        strncpy(buf, str, p - str);
        buf[p - str] = '\0';
        self = parse_addr(buf, flags);
        if(!self)
            return NULL;
        if(!parse_mask(self, p + 1, flags)) {
            free(self);
            return NULL;
        }
        return self;
    } else if((p = strchr(str, ','))) { /* new range character */
        NM top;
        int add;

        strncpy(buf, str, p - str);
        buf[p - str] = '\0';
        self = parse_addr(buf, flags);
        if(!self)
            return NULL;
        if(p[1] == '+')
            add = 1;
        else
            add = 0;
        top = parse_addr(p + add + 1, flags);
        if(!top) {
            free(self);
            return NULL;
        }
        if(add) {
            int carry;
            if(is_v4(top))
                top->neta.l &= 0xffffffffULL;
            top->neta = u128_add(self->neta, top->neta, &carry);
            if(carry) {
                free(self);
                free(top);
                return NULL;
            }
        }
        return nm_seq(self, top);
    } else if((self = parse_addr(str, flags))) {
        return self;
    } else if((p = strchr(str, ':'))) { /* old range character (sloppy) */
        NM top;
        int add;
        strncpy(buf, str, p - str);
        buf[p - str] = '\0';
        self = parse_addr(buf, flags);
        if(!self)
            return NULL;
        if(p[1] == '+') {
            add = 1;
            if(p[2] == '-') {
                /* this is a pretty special reverse compatibility
                 * situation.  N:+-5" would actually emit the range from
                 * N-5 to N because strtoul() hilariously accepts
                 * negative numbers and the original code never detected
                 * overflow and things just happened to work out. */
                struct in_addr s;
                char *endp;
                uint32_t v = self->neta.l + strtoul(p + 2, &endp, 0);
                if(*endp == '\0') {
                    s.s_addr = htonl(v);
                    top = nm_new_v4(&s);
                    if(!top) {
                        free(self);
                        return NULL;
                    }
                    return nm_seq(self, top);
                }
            }
        } else {
            add = 0;
        }

        top = parse_addr(p + add + 1, flags);
        if(!top) {
            free(self);
            return NULL;
        }
        if(add) {
            int carry;
            if(is_v4(top))
                top->neta.l &= 0xffffffffULL;
            top->neta = u128_add(self->neta, top->neta, &carry);
            if(carry) {
                free(self);
                free(top);
                return NULL;
            }
        }
        return nm_seq(self, top);
    } else {
        return NULL;
    }
}

NM nm_merge(NM dst, NM src) {
    /* both lists are ordered and non-overlapping.  Knit them into a
     * single ordered, non-overlapping list.  */
    NM tmp;
    NM *pos = &dst; /* double indirect pointer simplifies list insertion
                       logic. */
    while(src) {
        if(*pos == NULL) {
            /* remains of src go to tail of dst */
            tmp = src;
            src = *pos;
            *pos = tmp;
        } else if(subset_of(src, *pos)) {
            status("found %016llx %016llx/%d a subset of %016llx %016llx/%d", src->neta.h, src->neta.l, cidr(src->mask), (*pos)->neta.h, (*pos)->neta.l, cidr((*pos)->mask));
            /* drop src elt on the floor */
            if(src->domain != AF_INET) /* may need to promote domain */
                (*pos)->domain = src->domain;
            tmp = src;
            src = src->next;
            free(tmp);
        } else if(subset_of(*pos, src)) {
            /* src seems larger, merge the other direction instead */
            tmp = src;
            src = *pos;
            *pos = tmp;
        } else if(joinable_pair(src, *pos)) {
            status("joinable %016llx %016llx/%d and %016llx %016llx/%d", src->neta.h, src->neta.l, cidr(src->mask), (*pos)->neta.h, (*pos)->neta.l, cidr((*pos)->mask));
            /* pull the dst elt */
            tmp = *pos;
            *pos = (*pos)->next;
            if(src->domain == AF_INET)
                src->domain = tmp->domain;
            free(tmp);
            /* widen the src elt */
            src->mask = u128_lsh(src->mask, 1);
            src->neta = u128_and(src->neta, src->mask);
            /* and merge it back into the src tail */
            tmp = src->next;
            src->next = NULL;
            src = nm_merge(src, tmp);
            /* now the dst scan needs to start over to find preceding
             * join candidates. */
            pos = &dst;
            /* TODO: there should be a cheaper way to do this than an
             * effective double recursion, but the possibility of joins
             * cascading backwards makes this difficult */
        } else if(u128_cmp(src->neta, (*pos)->neta) < 0) {
            /* src elt goes here in dst list.  if top src elt were
             * spliced into dst, it may duplicate later elts in dst.
             * swap tails instead because src is well formed. */
            tmp = src;
            src = *pos;
            *pos = tmp;
        } else {
            /* move down the dst list */
            pos = &(*pos)->next;
        }
    }
    return dst;
}

void nm_walk(NM self, void (*cb)(int, nm_addr *, nm_addr *)) {
    int domain;
    nm_addr neta, mask;

    while(self) {
        neta.s6 = s6_of_u128(self->neta);
        mask.s6 = s6_of_u128(self->mask);
        if(is_v4(self)) {
            domain = AF_INET;
            neta.s.s_addr = htonl(
                    neta.s6.s6_addr[12] << 24 |
                    neta.s6.s6_addr[13] << 16 |
                    neta.s6.s6_addr[14] <<  8 |
                    neta.s6.s6_addr[15] <<  0);
            mask.s.s_addr = htonl(
                    mask.s6.s6_addr[12] << 24 |
                    mask.s6.s6_addr[13] << 16 |
                    mask.s6.s6_addr[14] <<  8 |
                    mask.s6.s6_addr[15] <<  0);
        } else {
            domain = AF_INET6;
        }
        cb(domain, &neta, &mask);
        self = self->next;
    }
}
