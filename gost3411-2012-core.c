/*
 * GOST R 34.11-2012 core and API functions.
 *
 * $Id$
 */

#include "gost3411-2012-core.h"

#define BSWAP64(x) \
    (((x & 0xFF00000000000000ULL) >> 56) | \
     ((x & 0x00FF000000000000ULL) >> 40) | \
     ((x & 0x0000FF0000000000ULL) >> 24) | \
     ((x & 0x000000FF00000000ULL) >>  8) | \
     ((x & 0x00000000FF000000ULL) <<  8) | \
     ((x & 0x0000000000FF0000ULL) << 24) | \
     ((x & 0x000000000000FF00ULL) << 40) | \
     ((x & 0x00000000000000FFULL) << 56))

void *
memalloc(const size_t size)
{
    void *p;

    /* Ensure p is on a 64-bit boundary. */ 
    if (posix_memalign(&p, (size_t) 64, size))
        err(EX_OSERR, NULL);

    return p;
}

void
GOST3411Destroy(GOST3411Context *CTX)
{
    memset(CTX->N, 0x00, sizeof uint512_u);
    memset(CTX->h, 0x00, sizeof uint512_u);
    memset(CTX->hash,   0x00, sizeof uint512_u);
    memset(CTX->Sigma,  0x00, sizeof uint512_u);
    memset(CTX->buffer, 0x00, sizeof uint512_u);
    memset(CTX->hexdigest, 0x00, sizeof (129));

    free(CTX->N);
    free(CTX->h);
    free(CTX->hash);
    free(CTX->Sigma);
    free(CTX->buffer);
    free(CTX->hexdigest);

    memset(CTX, 0x00, sizeof (GOST3411Context));
    free(CTX);
}

GOST3411Context *
GOST3411Init(const unsigned int digest_size)
{
    unsigned int i;
    GOST3411Context *CTX;

    CTX = memalloc(sizeof (GOST3411Context));

    CTX->N = memalloc(sizeof uint512_u);
    CTX->h = memalloc(sizeof uint512_u);
    CTX->hash = memalloc(sizeof uint512_u);
    CTX->Sigma = memalloc(sizeof uint512_u);
    CTX->buffer = memalloc(sizeof uint512_u);
    CTX->hexdigest = memalloc((size_t) 129);
    CTX->bufsize = 0;

    memcpy(CTX->N, &buffer0, sizeof buffer0);
    memcpy(CTX->h, &buffer0, sizeof buffer0);
    memcpy(CTX->hash, &buffer0, sizeof buffer0);
    memcpy(CTX->Sigma, &buffer0, sizeof buffer0);
    memcpy(CTX->buffer, &buffer0, sizeof buffer0);
    CTX->digest_size = digest_size;
    memset(CTX->hexdigest, 0, (size_t) 1);

    for (i = 0; i < 8; i++)
    {
        if (digest_size == 256)
            CTX->h->QWORD[i] = 0x0101010101010101ULL;
        else
            CTX->h->QWORD[i] = 0x00ULL;
    }

    return CTX;
}

static inline void
pad(GOST3411Context *CTX)
{
    unsigned char buf[64];

    if (CTX->bufsize > 63)
        return;

    memset(&buf, 0x00, sizeof buf);
    memcpy(&buf, CTX->buffer, CTX->bufsize);

    buf[CTX->bufsize] = 0x01;
    memcpy(CTX->buffer, &buf, sizeof buf);
}

static inline void
add512(const union uint512_u *x, const union uint512_u *y, union uint512_u *r)
{
#ifndef __GOST3411_BIG_ENDIAN__
    unsigned int CF, OF;
    unsigned int i;

    CF = 0;
    for (i = 0; i < 8; i++)
    {
        r->QWORD[i] = x->QWORD[i] + y->QWORD[i];
        if ( (r->QWORD[i] < y->QWORD[i]) || 
             (r->QWORD[i] < x->QWORD[i]) )
            OF = 1;
        else
            OF = 0;

        r->QWORD[i] += CF;
        CF = OF;
    }
#else
    const unsigned char *xp, *yp;
    unsigned char *rp;
    unsigned int i;
    int buf;

    xp = (const unsigned char *) &x[0];
    yp = (const unsigned char *) &y[0];
    rp = (unsigned char *) &r[0];

    buf = 0;
    for (i = 0; i < 64; i++)
    {
        buf = xp[i] + yp[i] + (buf >> 8);
        rp[i] = (unsigned char) buf & 0xFF;
    }
#endif
}

static void
g(union uint512_u *h, const union uint512_u *N, const union uint512_u *m)
{
#ifdef __GOST3411_HAS_SSE2__
    __m128i xmm0, xmm2, xmm4, xmm6; /* XMMR0-quadruple */
    __m128i xmm1, xmm3, xmm5, xmm7; /* XMMR1-quadruple */
    unsigned int i;

    LOAD(N, xmm0, xmm2, xmm4, xmm6);
    XLPS128M(h, xmm0, xmm2, xmm4, xmm6);

    LOAD(m, xmm1, xmm3, xmm5, xmm7);
    XLPS128R(xmm0, xmm2, xmm4, xmm6, xmm1, xmm3, xmm5, xmm7);

    for (i = 0; i < 11; i++)
        ROUND128(i, xmm0, xmm2, xmm4, xmm6, xmm1, xmm3, xmm5, xmm7);

    XLPS128M((&C[11]), xmm0, xmm2, xmm4, xmm6);
    X128R(xmm0, xmm2, xmm4, xmm6, xmm1, xmm3, xmm5, xmm7);

    X128M(h, xmm0, xmm2, xmm4, xmm6);
    X128M(m, xmm0, xmm2, xmm4, xmm6);

    UNLOAD(h, xmm0, xmm2, xmm4, xmm6);

    /* Restore the Floating-point status on the CPU */
    _mm_empty();
#else
    union uint512_u Ki, data;
    unsigned int i;

    XLPS(h, N, (&data));

    /* Starting E() */
    Ki = data;
    XLPS((&Ki), m, (&data));

    for (i = 0; i < 11; i++)
        ROUND(i, (&Ki), (&data));

    XLPS((&Ki), (&C[11]), (&Ki));
    X((&Ki), (&data), (&data));
    /* E() done */

    X((&data), h, (&data));
    X((&data), m, h);
#endif
}

static inline void
round2(GOST3411Context *CTX)
{
    g(CTX->h, CTX->N, CTX->buffer);

    add512(CTX->N, &buffer512, CTX->N);
    add512(CTX->Sigma, CTX->buffer, CTX->Sigma);
}

static inline void
round3(GOST3411Context *CTX)
{
    union uint512_u buf;

    memset(&buf, 0x00, sizeof buf);
    memcpy(&buf, CTX->buffer, CTX->bufsize);
    memcpy(CTX->buffer, &buf, sizeof uint512_u);

    memset(&buf, 0x00, sizeof buf);
#ifndef __GOST3411_BIG_ENDIAN__
    buf.QWORD[0] = CTX->bufsize << 3;
#else
    buf.QWORD[0] = BSWAP64(CTX->bufsize << 3);
#endif

    pad(CTX);

    g(CTX->h, CTX->N, CTX->buffer);

    add512(CTX->N, &buf, CTX->N);
    add512(CTX->Sigma, CTX->buffer, CTX->Sigma);

    g(CTX->h, &buffer0, CTX->N);

    g(CTX->h, &buffer0, CTX->Sigma);
    memcpy(CTX->hash, CTX->h, sizeof uint512_u);
}

void
GOST3411Update(GOST3411Context *CTX, const unsigned char *data, size_t len)
{
    size_t chunksize;

    while (len)
    {
        chunksize = 64 - CTX->bufsize;
        if (chunksize > len)
            chunksize = len;

        memcpy(CTX->buffer, data, chunksize);

        CTX->bufsize += chunksize;
        data += chunksize;
        len -= chunksize;

        if (CTX->bufsize == 64)
        {
            round2(CTX);
            CTX->bufsize = 0;
        }
    }
}

void
GOST3411Final(GOST3411Context *CTX)
{
    int i, j;
    char *buf;

    round3(CTX);
    CTX->bufsize = 0;

    buf = memalloc((size_t) 17);

    if (CTX->digest_size == 256)
        j = 4;
    else
        j = 0;

    i = 7;
    while (i >= j)
    {
#ifndef __GOST3411_BIG_ENDIAN__
        snprintf(buf, (size_t) 17, "%.16llx", CTX->hash->QWORD[i]);
#else
        snprintf(buf, (size_t) 17, "%.16llx", BSWAP64(CTX->hash->QWORD[i]));
#endif
        strncat(CTX->hexdigest, buf, (size_t) 16);
        i--;
    }

    free(buf);
}
