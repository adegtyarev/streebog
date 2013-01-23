/*
 * GOST R 34.11-2012 core and API functions.
 *
 * $Id$
 */

#include "gost3411-2012-core.h"

#define BSWAP64(x) \
    ((((x) & 0xFF00000000000000ULL) >> 56) | \
     (((x) & 0x00FF000000000000ULL) >> 40) | \
     (((x) & 0x0000FF0000000000ULL) >> 24) | \
     (((x) & 0x000000FF00000000ULL) >>  8) | \
     (((x) & 0x00000000FF000000ULL) <<  8) | \
     (((x) & 0x0000000000FF0000ULL) << 24) | \
     (((x) & 0x000000000000FF00ULL) << 40) | \
     (((x) & 0x00000000000000FFULL) << 56))

/* 64-bit alignment required on 32-bit systems to produce optimized pxor
 * sequence in XLPS */
static unsigned long long __attribute__((aligned(8))) Ax[8][256];

typedef struct Ai_t
{
    unsigned char i[4];
} Ai_t;

static const Ai_t Ai[16] = {
    {{ 65, 65, 65, 65}},
    {{  3, 65, 65, 65}},
    {{  2, 65, 65, 65}},
    {{  3,  2, 65, 65}},
    {{  1, 65, 65, 65}},
    {{  3,  1, 65, 65}},
    {{  2,  1, 65, 65}},
    {{  3,  2,  1, 65}},
    {{  0, 65, 65, 65}},
    {{  3,  0, 65, 65}},
    {{  2,  0, 65, 65}},
    {{  3,  2,  0, 65}},
    {{  1,  0, 65, 65}},
    {{  3,  1,  0, 65}},
    {{  2,  1,  0, 65}},
    {{  3,  2,  1,  0}}
};

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
destroy(GOST3411Context *CTX)
{
    free(CTX->N);
    free(CTX->h);
    free(CTX->hash);
    free(CTX->Sigma);
    free(CTX->buffer);
    free(CTX->hexdigest);
    free(CTX);
}

GOST3411Context *
init(const uint32_t digest_size)
{
    unsigned int i, j, b;
    Ai_t idx1, idx2;
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
            CTX->h->QWORD[i] = 0ULL;
    }

    for (i = 0; i < 8; i++)
    {
        for (b = 0; b < 256; b++)
        {
            j = 64 - (i << 3) - 8;
            idx1 = Ai[(b & 0x0F) >> 0];
            idx2 = Ai[(b & 0xF0) >> 4];
            Ax[i][b] = A[j + 4 + idx1.i[0]] ^ A[j + idx2.i[0]] ^
                       A[j + 4 + idx1.i[1]] ^ A[j + idx2.i[1]] ^
                       A[j + 4 + idx1.i[2]] ^ A[j + idx2.i[2]] ^
                       A[j + 4 + idx1.i[3]] ^ A[j + idx2.i[3]];
        }
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
#ifdef __GOST3411_LITTLE_ENDIAN__
    unsigned int CF;
    unsigned int i;

    CF = 0;
    for (i = 0; i < 8; i++)
    {
        r->QWORD[i] = x->QWORD[i] + y->QWORD[i] + CF;
        if ( (r->QWORD[i] < y->QWORD[i]) || 
             (r->QWORD[i] < x->QWORD[i]) )
            CF = 1;
        else
            CF = 0;
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
#ifdef __GOST3411_LITTLE_ENDIAN__
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
update(GOST3411Context *CTX, const char *data, size_t len)
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
final(GOST3411Context *CTX)
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
#ifdef __GOST3411_LITTLE_ENDIAN__
        snprintf(buf, (size_t) 17, "%.16llx", CTX->hash->QWORD[i]);
#else
        snprintf(buf, (size_t) 17, "%.16llx", BSWAP64(CTX->hash->QWORD[i]));
#endif
        strncat(CTX->hexdigest, buf, (size_t) 16);
        i--;
    }

    free(buf);
}
