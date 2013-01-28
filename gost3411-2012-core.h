/*
 * $Id$
 */

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "config.h"

#if defined _MSC_VER
#define ALIGN(x) __declspec(align(x))
#else
#define ALIGN(x) __attribute__ ((__aligned__(x)))
#endif

#if defined   __GOST3411_HAS_SSE41__
#include "gost3411-2012-sse41.h"
#elif defined __GOST3411_HAS_SSE2__
#include "gost3411-2012-sse2.h"
#elif defined __GOST3411_HAS_MMX__ 
#include "gost3411-2012-mmx.h"
#else
#include "gost3411-2012-ref.h"
#endif

#define DEFAULT_DIGEST_SIZE 512
#define ALGNAME "GOST R 34.11-2012"

ALIGN(16) union uint512_u
{
    unsigned long long QWORD[8];
} uint512_u;

#include "gost3411-2012-const.h"
#include "gost3411-2012-precalc.h"

ALIGN(16) typedef struct GOST3411Context
{
    unsigned char *buffer;
    union uint512_u *hash;
    union uint512_u *h;
    union uint512_u *N;
    union uint512_u *Sigma;
    size_t bufsize;
    unsigned int digest_size;
    char *hexdigest;
} GOST3411Context;

void
*memalloc(const size_t size);

GOST3411Context * 
GOST3411Init(const unsigned int digest_size);

void
GOST3411Update(GOST3411Context *CTX, const unsigned char *data, size_t len);

void
GOST3411Final(GOST3411Context *CTX);

void
GOST3411Destroy(GOST3411Context *CTX);
