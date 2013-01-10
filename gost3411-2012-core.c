/*
 * $Id$
 */

#include <gost3411-2012-core.h>

#define X(x, y, z) { \
    z->QWORD[0] = x->QWORD[0] ^ y->QWORD[0]; \
    z->QWORD[1] = x->QWORD[1] ^ y->QWORD[1]; \
    z->QWORD[2] = x->QWORD[2] ^ y->QWORD[2]; \
    z->QWORD[3] = x->QWORD[3] ^ y->QWORD[3]; \
    z->QWORD[4] = x->QWORD[4] ^ y->QWORD[4]; \
    z->QWORD[5] = x->QWORD[5] ^ y->QWORD[5]; \
    z->QWORD[6] = x->QWORD[6] ^ y->QWORD[6]; \
    z->QWORD[7] = x->QWORD[7] ^ y->QWORD[7]; \
}

#define XLPS(x, y, data) { \
    unsigned long long r0, r1, r2, r3, r4, r5, r6, r7; \
    int i; \
    \
    r0 = x->QWORD[0] ^ y->QWORD[0]; \
    r1 = x->QWORD[1] ^ y->QWORD[1]; \
    r2 = x->QWORD[2] ^ y->QWORD[2]; \
    r3 = x->QWORD[3] ^ y->QWORD[3]; \
    r4 = x->QWORD[4] ^ y->QWORD[4]; \
    r5 = x->QWORD[5] ^ y->QWORD[5]; \
    r6 = x->QWORD[6] ^ y->QWORD[6]; \
    r7 = x->QWORD[7] ^ y->QWORD[7]; \
    \
    data->QWORD[0]  = Ax[0][Pi[r0 & 0xFF]]; \
    data->QWORD[0] ^= Ax[1][Pi[r1 & 0xFF]]; \
    data->QWORD[0] ^= Ax[2][Pi[r2 & 0xFF]]; \
    data->QWORD[0] ^= Ax[3][Pi[r3 & 0xFF]]; \
    data->QWORD[0] ^= Ax[4][Pi[r4 & 0xFF]]; \
    data->QWORD[0] ^= Ax[5][Pi[r5 & 0xFF]]; \
    data->QWORD[0] ^= Ax[6][Pi[r6 & 0xFF]]; \
    data->QWORD[0] ^= Ax[7][Pi[r7 & 0xFF]]; \
    \
    for (i = 1; i < 8; i++) \
    {\
        r0 >>= 0x8; \
        r1 >>= 0x8; \
        r2 >>= 0x8; \
        r3 >>= 0x8; \
        r4 >>= 0x8; \
        r5 >>= 0x8; \
        r6 >>= 0x8; \
        r7 >>= 0x8; \
        \
        data->QWORD[i]  = Ax[0][Pi[r0 & 0xFF]]; \
        data->QWORD[i] ^= Ax[1][Pi[r1 & 0xFF]]; \
        data->QWORD[i] ^= Ax[2][Pi[r2 & 0xFF]]; \
        data->QWORD[i] ^= Ax[3][Pi[r3 & 0xFF]]; \
        data->QWORD[i] ^= Ax[4][Pi[r4 & 0xFF]]; \
        data->QWORD[i] ^= Ax[5][Pi[r5 & 0xFF]]; \
        data->QWORD[i] ^= Ax[6][Pi[r6 & 0xFF]]; \
        data->QWORD[i] ^= Ax[7][Pi[r7 & 0xFF]]; \
    }\
}

#define ROUND(i, Ki, data) { \
    K(i, Ki); \
    XLPS(Ki, data, data); \
}

static const union uint512_u buffer512  = {{ 512ULL, 0ULL, 0ULL, 0ULL, 0ULL,
    0ULL, 0ULL, 0ULL }};

static const union uint512_u buffer0    = {{   0ULL, 0ULL, 0ULL, 0ULL, 0ULL,
    0ULL, 0ULL, 0ULL }};

static const union uint512_u C[12] = {
    {{
          0xdd806559f2a64507ULL,
          0x05767436cc744d23ULL,
          0xa2422a08a460d315ULL,
          0x4b7ce09192676901ULL,
          0x714eb88d7585c4fcULL,
          0x2f6a76432e45d016ULL,
          0xebcb2f81c0657c1fULL,
          0xb1085bda1ecadae9ULL
    }},
    {{
          0xe679047021b19bb7ULL,
          0x55dda21bd7cbcd56ULL,
          0x5cb561c2db0aa7caULL,
          0x9ab5176b12d69958ULL,
          0x61d55e0f16b50131ULL,
          0xf3feea720a232b98ULL,
          0x4fe39d460f70b5d7ULL,
          0x6fa3b58aa99d2f1aULL
    }},
    {{
          0x991e96f50aba0ab2ULL,
          0xc2b6f443867adb31ULL,
          0xc1c93a376062db09ULL,
          0xd3e20fe490359eb1ULL,
          0xf2ea7514b1297b7bULL,
          0x06f15e5f529c1f8bULL,
          0x0a39fc286a3d8435ULL,
          0xf574dcac2bce2fc7ULL
    }},
    {{
          0x220cbebc84e3d12eULL,
          0x3453eaa193e837f1ULL,
          0xd8b71333935203beULL,
          0xa9d72c82ed03d675ULL,
          0x9d721cad685e353fULL,
          0x488e857e335c3c7dULL,
          0xf948e1a05d71e4ddULL,
          0xef1fdfb3e81566d2ULL
    }},
    {{
          0x601758fd7c6cfe57ULL,
          0x7a56a27ea9ea63f5ULL,
          0xdfff00b723271a16ULL,
          0xbfcd1747253af5a3ULL,
          0x359e35d7800fffbdULL,
          0x7f151c1f1686104aULL,
          0x9a3f410c6ca92363ULL,
          0x4bea6bacad474799ULL
    }},
    {{
          0xfa68407a46647d6eULL,
          0xbf71c57236904f35ULL,
          0x0af21f66c2bec6b6ULL,
          0xcffaa6b71c9ab7b4ULL,
          0x187f9ab49af08ec6ULL,
          0x2d66c4f95142a46cULL,
          0x6fa4c33b7a3039c0ULL,
          0xae4faeae1d3ad3d9ULL
    }},
    {{
          0x8886564d3a14d493ULL,
          0x3517454ca23c4af3ULL,
          0x06476983284a0504ULL,
          0x0992abc52d822c37ULL,
          0xd3473e33197a93c9ULL,
          0x399ec6c7e6bf87c9ULL,
          0x51ac86febf240954ULL,
          0xf4c70e16eeaac5ecULL
    }},
    {{
          0xa47f0dd4bf02e71eULL,
          0x36acc2355951a8d9ULL,
          0x69d18d2bd1a5c42fULL,
          0xf4892bcb929b0690ULL,
          0x89b4443b4ddbc49aULL,
          0x4eb7f8719c36de1eULL,
          0x03e7aa020c6e4141ULL,
          0x9b1f5b424d93c9a7ULL
    }},
    {{
          0x7261445183235adbULL,
          0x0e38dc92cb1f2a60ULL,
          0x7b2b8a9aa6079c54ULL,
          0x800a440bdbb2ceb1ULL,
          0x3cd955b7e00d0984ULL,
          0x3a7d3a1b25894224ULL,
          0x944c9ad8ec165fdeULL,
          0x378f5a541631229bULL
    }},
    {{
          0x74b4c7fb98459cedULL,
          0x3698fad1153bb6c3ULL,
          0x7a1e6c303b7652f4ULL,
          0x9fe76702af69334bULL,
          0x1fffe18a1b336103ULL,
          0x8941e71cff8a78dbULL,
          0x382ae548b2e4f3f3ULL,
          0xabbedea680056f52ULL
    }},
    {{
          0x6bcaa4cd81f32d1bULL,
          0xdea2594ac06fd85dULL,
          0xefbacd1d7d476e98ULL,
          0x8a1d71efea48b9caULL,
          0x2001802114846679ULL,
          0xd8fa6bbbebab0761ULL,
          0x3002c6cd635afe94ULL,
          0x7bcd9ed0efc889fbULL
    }},
    {{
          0x48bc924af11bd720ULL,
          0xfaf417d5d9b21b99ULL,
          0xe71da4aa88e12852ULL,
          0x5d80ef9d1891cc86ULL,
          0xf82012d430219f9bULL,
          0xcda43c32bcdf1d77ULL,
          0xd21380b00449b17aULL,
          0x378ee767f11631baULL
    }}
};

static const unsigned long long A[129] = { 
    0x8e20faa72ba0b470ULL, 0x47107ddd9b505a38ULL, 0xad08b0e0c3282d1cULL,
    0xd8045870ef14980eULL, 0x6c022c38f90a4c07ULL, 0x3601161cf205268dULL,
    0x1b8e0b0e798c13c8ULL, 0x83478b07b2468764ULL, 0xa011d380818e8f40ULL,
    0x5086e740ce47c920ULL, 0x2843fd2067adea10ULL, 0x14aff010bdd87508ULL,
    0x0ad97808d06cb404ULL, 0x05e23c0468365a02ULL, 0x8c711e02341b2d01ULL,
    0x46b60f011a83988eULL, 0x90dab52a387ae76fULL, 0x486dd4151c3dfdb9ULL,
    0x24b86a840e90f0d2ULL, 0x125c354207487869ULL, 0x092e94218d243cbaULL,
    0x8a174a9ec8121e5dULL, 0x4585254f64090fa0ULL, 0xaccc9ca9328a8950ULL,
    0x9d4df05d5f661451ULL, 0xc0a878a0a1330aa6ULL, 0x60543c50de970553ULL,
    0x302a1e286fc58ca7ULL, 0x18150f14b9ec46ddULL, 0x0c84890ad27623e0ULL,
    0x0642ca05693b9f70ULL, 0x0321658cba93c138ULL, 0x86275df09ce8aaa8ULL,
    0x439da0784e745554ULL, 0xafc0503c273aa42aULL, 0xd960281e9d1d5215ULL,
    0xe230140fc0802984ULL, 0x71180a8960409a42ULL, 0xb60c05ca30204d21ULL,
    0x5b068c651810a89eULL, 0x456c34887a3805b9ULL, 0xac361a443d1c8cd2ULL,
    0x561b0d22900e4669ULL, 0x2b838811480723baULL, 0x9bcf4486248d9f5dULL,
    0xc3e9224312c8c1a0ULL, 0xeffa11af0964ee50ULL, 0xf97d86d98a327728ULL,
    0xe4fa2054a80b329cULL, 0x727d102a548b194eULL, 0x39b008152acb8227ULL,
    0x9258048415eb419dULL, 0x492c024284fbaec0ULL, 0xaa16012142f35760ULL,
    0x550b8e9e21f7a530ULL, 0xa48b474f9ef5dc18ULL, 0x70a6a56e2440598eULL,
    0x3853dc371220a247ULL, 0x1ca76e95091051adULL, 0x0edd37c48a08a6d8ULL,
    0x07e095624504536cULL, 0x8d70c431ac02a736ULL, 0xc83862965601dd1bULL,
    0x641c314b2b8ee083ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
    0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
};

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

static unsigned long long Ax[8][256];

static const unsigned int Tau[64] = {
    0,   8,  16,  24,  32,  40,  48,  56, 
    1,   9,  17,  25,  33,  41,  49,  57, 
    2,  10,  18,  26,  34,  42,  50,  58, 
    3,  11,  19,  27,  35,  43,  51,  59, 
    4,  12,  20,  28,  36,  44,  52,  60, 
    5,  13,  21,  29,  37,  45,  53,  61, 
    6,  14,  22,  30,  38,  46,  54,  62, 
    7,  15,  23,  31,  39,  47,  55,  63
};

static const unsigned int Pi[256] = {
    252, 238, 221,  17, 207, 110,  49,  22, 
    251, 196, 250, 218,  35, 197,   4,  77, 
    233, 119, 240, 219, 147,  46, 153, 186, 
     23,  54, 241, 187,  20, 205,  95, 193, 
    249,  24, 101,  90, 226,  92, 239,  33, 
    129,  28,  60,  66, 139,   1, 142,  79, 
      5, 132,   2, 174, 227, 106, 143, 160, 
      6,  11, 237, 152, 127, 212, 211,  31, 
    235,  52,  44,  81, 234, 200,  72, 171, 
    242,  42, 104, 162, 253,  58, 206, 204, 
    181, 112,  14,  86,   8,  12, 118,  18, 
    191, 114,  19,  71, 156, 183,  93, 135, 
     21, 161, 150,  41,  16, 123, 154, 199, 
    243, 145, 120, 111, 157, 158, 178, 177, 
     50, 117,  25,  61, 255,  53, 138, 126, 
    109,  84, 198, 128, 195, 189,  13,  87, 
    223, 245,  36, 169,  62, 168,  67, 201, 
    215, 121, 214, 246, 124,  34, 185,   3, 
    224,  15, 236, 222, 122, 148, 176, 188, 
    220, 232,  40,  80,  78,  51,  10,  74, 
    167, 151,  96, 115,  30,   0,  98,  68, 
     26, 184,  56, 130, 100, 159,  38,  65, 
    173,  69,  70, 146,  39,  94,  85,  47, 
    140, 163, 165, 125, 105, 213, 149,  59, 
      7,  88, 179,  64, 134, 172,  29, 247, 
     48,  55, 107, 228, 136, 217, 231, 137, 
    225,  27, 131,  73,  76,  63, 248, 254, 
    141,  83, 170, 144, 202, 216, 133,  97, 
     32, 113, 103, 164,  45,  43,   9,  91, 
    203, 155,  37, 208, 190, 229, 108,  82, 
     89, 166, 116, 210, 230, 244, 180, 192, 
    209, 102, 175, 194,  57,  75,  99, 182
};

void *
memalloc(const size_t size)
{
    char *p;
    long offset;

    if ((p = malloc(size)) == NULL)
        err(EX_OSERR, NULL);

    /* Ensure p is on a 64-bit boundary. */ 
    if ((offset = (long) p & 7L))
        p += 8 - offset;

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
init(const unsigned int digest_size)
{
    int i, j, b;
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
            idx1 = Ai[b & 0x0F];
            idx2 = Ai[(b & 0xF0) >> 4];
            Ax[i][b] = A[j + 4 + idx1.i[0]] ^ A[j + idx2.i[0]] ^
                       A[j + 4 + idx1.i[1]] ^ A[j + idx2.i[1]] ^
                       A[j + 4 + idx1.i[2]] ^ A[j + idx2.i[2]] ^
                       A[j + 4 + idx1.i[3]] ^ A[j + idx2.i[3]];
        }
    }

    return CTX;
}

static void
pad(union uint512_u *data)
{
    int i;

    i = 64;
    do
    {
        i--;
        if (data->QWORD[i >> 3] == 0)
        {
            if (i >> 3 == 0)
            {
                data->BYTE[0] = 0x01;
                break;
            }

            i = (int) ((i >> 3) << 3);
            continue;
        }
        if (data->BYTE[i])
        {
            data->BYTE[++i] = 0x01;
            break;
        }
    }
    while (i > 0);
}

static inline void
K(const int k, union uint512_u *Ki)
{
    XLPS(Ki, (&C[k]), Ki);
}

static inline void
E(const union uint512_u *Key, const union uint512_u *m, union uint512_u *data)
{
    union uint512_u Ki;

    Ki = (*Key);

    XLPS((&Ki), m, data);

    ROUND( 0, (&Ki), data);
    ROUND( 1, (&Ki), data);
    ROUND( 2, (&Ki), data);
    ROUND( 3, (&Ki), data);
    ROUND( 4, (&Ki), data);
    ROUND( 5, (&Ki), data);
    ROUND( 6, (&Ki), data);
    ROUND( 7, (&Ki), data);
    ROUND( 8, (&Ki), data);
    ROUND( 9, (&Ki), data);
    ROUND(10, (&Ki), data);

    K(11, &Ki);

    X((&Ki), data, data);
}

static inline void
add512(const union uint512_u *x, const union uint512_u *y, union uint512_u *r)
{
    int i;
    unsigned long long CF;

    CF = 0;
    for (i = 0; i < 8; i++)
    {
        r->QWORD[i] = x->QWORD[i] + y->QWORD[i] + CF;
        /* Actually it is sufficient to compare *r with ANY argument. However,
         * one argument of add512() call MAY point to *r at the same time.
         * Consider add512(x, y, x) or ad512(x, y, y). We avoid confusion by
         * comparing *r with both arguments. Instead of *y, assume that *x
         * probably points to *r, so *y goes first.
         */
        if ( (r->QWORD[i] < y->QWORD[i]) || 
             (r->QWORD[i] < x->QWORD[i]) )
            CF = 1;
        else
            CF = 0;
    }
}

static void
g(const union uint512_u *N, const union uint512_u *h,
        const union uint512_u *m, union uint512_u *data)
{
    XLPS(h, N, data);
    E(data, m, data);
    X(data, h, data);
    X(data, m, data);
}

static inline void
round2(GOST3411Context *CTX)
{
    g(CTX->N, CTX->h, CTX->buffer, CTX->hash);

    memcpy(CTX->h, CTX->hash, sizeof uint512_u);

    add512(CTX->N, &buffer512, CTX->N);
    add512(CTX->Sigma, CTX->buffer, CTX->Sigma);
}

static inline void
round3(GOST3411Context *CTX)
{
    union uint512_u buf;

    buf = buffer0;
    memcpy(&buf, CTX->buffer, CTX->bufsize);
    memcpy(CTX->buffer, &buf, sizeof uint512_u);

    buf = buffer0;
    buf.QWORD[0] = CTX->bufsize * 8;

    pad(CTX->buffer);

    g(CTX->N, CTX->h, CTX->buffer, CTX->hash);
    memcpy(CTX->h, CTX->hash, sizeof uint512_u);

    add512(CTX->N, &buf, CTX->N);
    add512(CTX->Sigma, CTX->buffer, CTX->Sigma);
        
    g(&buffer0, CTX->h, CTX->N, CTX->hash); 
    memcpy(CTX->h, CTX->hash, sizeof uint512_u);

    g(&buffer0, CTX->h, CTX->Sigma, CTX->hash);
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

        while (CTX->bufsize == 64)
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

    buf = memalloc((size_t)17);

    if (CTX->digest_size == 256)
        j = 4;
    else
        j = 0;

    i = 7;
    while (i >= j)
    {
        snprintf(buf, (size_t) 17, "%.16llx", CTX->hash->QWORD[i]);
        strncat(CTX->hexdigest, buf, (size_t) 16);
        i--;
    }

    free(buf);
}
