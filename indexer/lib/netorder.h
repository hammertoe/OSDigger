#ifndef NETORDER_H
#define NETORDER_H

#include "sysfuncs.h"

/* [RPAP - Feb 97: WIN32 Port] */
#ifdef __WIN32__
#include "win32in.h"
#else
# include <netinet/in.h>
#endif

#if !defined(WORDS_BIGENDIAN)		/* then LITTLE_ENDIAN */

#define X86SWAP(__X) \
__extension__ ({ __asm ("bswap %0" \
        : "=r" (__X) \
        : "0" (__X)); \
   __X; })

/* double */
#define HTOND(d)                                                                                  \
        do {                                                                                      \
             unsigned long tmph, tmpl;                                                            \
	     bcopy ((char *) &d, (char *) &tmph, sizeof(double) >> 1);                            \
	     bcopy ((char *) &d + (sizeof(double) >> 1), (char *) &tmpl, sizeof (double) >> 1);   \
	     tmph = htonl (tmph);                                                                 \
	     tmpl = htonl (tmpl);                                                                 \
	     bcopy ((char *) &tmpl, (char *) &d, sizeof (double) >> 1);                           \
	     bcopy ((char *) &tmph, (char *) &d + (sizeof(double) >> 1), sizeof (double) >> 1);   \
	}while(0)
#define NTOHD(d)                                                                                  \
        do {                                                                                      \
             unsigned long tmph, tmpl;                                                            \
	     bcopy ((char *) &d, (char *) &tmph, sizeof(double) >> 1);                            \
	     bcopy ((char *) &d + (sizeof(double) >> 1), (char *) &tmpl, sizeof (double) >> 1);   \
	     tmph = ntohl (tmph);                                                                 \
	     tmpl = ntohl (tmpl);                                                                 \
	     bcopy ((char *) &tmpl, (char *) &d, sizeof (double) >> 1);                           \
	     bcopy ((char *) &tmph, (char *) &d + (sizeof(double) >> 1), sizeof (double) >> 1);   \
        }while(0)
#define HTOND2(hd, nd)                                                                            \
        do {                                                                                      \
             unsigned long tmph, tmpl;                                                            \
	     bcopy ((char *) &hd, (char *) &tmph, sizeof(double) >> 1);                           \
	     bcopy ((char *) &hd + (sizeof(double) >> 1), (char *) &tmpl, sizeof (double) >> 1);  \
	     tmph = htonl (tmph);                                                                 \
	     tmpl = htonl (tmpl);                                                                 \
	     bcopy ((char *) &tmpl, (char *) &nd, sizeof (double) >> 1);                          \
	     bcopy ((char *) &tmph, (char *) &nd + (sizeof(double) >> 1), sizeof (double) >> 1);  \
	}while(0)
#define NTOHD2(nd, hd)                                                                            \
        do {                                                                                      \
             unsigned long tmph, tmpl;                                                            \
	     bcopy ((char *) &nd, (char *) &tmph, sizeof(double) >> 1);                           \
	     bcopy ((char *) &nd + (sizeof(double) >> 1), (char *) &tmpl, sizeof (double) >> 1);  \
	     tmph = ntohl (tmph);                                                                 \
	     tmpl = ntohl (tmpl);                                                                 \
	     bcopy ((char *) &tmpl, (char *) &hd, sizeof (double) >> 1);                          \
	     bcopy ((char *) &tmph, (char *) &hd + (sizeof(double) >> 1), sizeof (double) >> 1);  \
        }while(0)

/* float */
#define HTONF(f)          X86SWAP(f)
#define NTOHF(f)          X86SWAP(f)
#define HTONF2(hf, nf)    do {nf = hf; X86SWAP(nf);} while(0)
#define NTOHF2(nf, hf)    do {hf = nf; X86SWAP(hf);} while(0)

/* pointers */
#define HTONP(p)          X86SWAP(p)
#define NTOHP(p)          X86SWAP(p)
#define HTONP2(hp, np)    do {np = hp; X86SWAP(np);} while(0)
#define NTOHP2(np, hp)    do {hp = np; X86SWAP(hp);} while(0)

/* unsigned long */
#define HTONUL(l)         X86SWAP(l)
#define NTOHUL(l)         X86SWAP(l)
#define HTONUL2(hl, nl)   do {nl = hl; X86SWAP(nl);} while(0)
#define NTOHUL2(nl, hl)   do {hl = nl; X86SWAP(hl);} while(0)

/* signed long */
#define HTONSL(l)         X86SWAP(l)
#define NTOHSL(l)         X86SWAP(l)
#define HTONSL2(hl, nl)   do {nl = hl; X86SWAP(nl);} while(0)
#define NTOHSL2(nl, hl)   do {hl = nl; X86SWAP(hl);} while(0)

/* unsigned int */
#define HTONUI(i)         X86SWAP(i)
#define NTOHUI(i)         X86SWAP(i)
#define HTONUI2(hi, ni)   do {ni = hi; X86SWAP(ni);} while(0)
#define NTOHUI2(ni, hi)   do {hi = ni; X86SWAP(hi);} while(0)

/* signed int */
#define HTONSI(i)         X86SWAP(i)
#define NTOHSI(i)         X86SWAP(i)
#define HTONSI2(hi, ni)   do {ni = hi; X86SWAP(ni);} while(0)
#define NTOHSI2(ni, hi)   do {hi = ni; X86SWAP(hi);} while(0)

/* unsigned short */
#define HTONUS(s)         ((s) = htons((s)))
#define NTOHUS(s)         ((s) = ntohs((s)))
#define HTONUS2(hs, ns)   ((ns) = htons((hs)))
#define NTOHUS2(ns, hs)   ((hs) = ntohs((ns)))

#else   /* _BIG_ENDIAN */

/* double */
#define HTOND(d)          (d)
#define NTOHD(d)          (d)
#define HTOND2(hd, nd)    ((hd) = (nd))
#define NTOHD2(nd, hd)    ((nd) = (hd))

/* float */
#define HTONF(f)          (f)
#define NTOHF(f)          (f)
#define HTONF2(hf, nf)    ((nf) = (hf))
#define NTOHF2(nf, hf)    ((hf) = (nf))

/* pointers */
#define HTONP(p)          (p)
#define NTOHP(p)          (p)
#define HTONP2(hp, np)    ((np) = (hp))
#define NTOHP2(np, hp)    ((hp) = (np))

/* unsigned long */
#define HTONUL(l)         (l)
#define NTOHUL(l)         (l)
#define HTONUL2(hl, nl)   ((nl) = (hl))
#define NTOHUL2(nl, hl)   ((hl) = (nl))

/* signed long */
#define HTONSL(l)         (l)
#define NTOHSL(l)         (l)
#define HTONSL2(hl, nl)   ((nl) = (hl))
#define NTOHSL2(nl, hl)   ((hl) = (nl))

/* unsigned int */
#define HTONUI(i)         (i)
#define NTOHUI(i)         (i)
#define HTONUI2(hi, ni)   ((ni) = (hi))
#define NTOHUI2(ni, hi)   ((hi) = (ni))

/* signed int */
#define HTONSI(i)         (i)
#define NTOHSI(i)         (i)
#define HTONSI2(hi, ni)   ((ni) = (hi))
#define NTOHSI2(ni, hi)   ((hi) = (ni))

/* unsigned short */
#define HTONUS(s)         (s)
#define NTOHUS(s)         (s)
#define HTONUS2(hs, ns)   ((ns) = (hs))
#define NTOHUS2(ns, hs)   ((hs) = (ns))



#endif

#endif /* netorder.h */
