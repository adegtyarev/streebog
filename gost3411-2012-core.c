/*
 * $Id$
 */

#include <gost3411-2012-core.h>

#define X(x, y, z) { \
    z->word[0] = x->word[0] ^ y->word[0]; \
    z->word[1] = x->word[1] ^ y->word[1]; \
    z->word[2] = x->word[2] ^ y->word[2]; \
    z->word[3] = x->word[3] ^ y->word[3]; \
    z->word[4] = x->word[4] ^ y->word[4]; \
    z->word[5] = x->word[5] ^ y->word[5]; \
    z->word[6] = x->word[6] ^ y->word[6]; \
    z->word[7] = x->word[7] ^ y->word[7]; \
}

#define XR(x, y, z) { \
    z->word[0] = x->word[0] ^ y->word[7]; \
    z->word[1] = x->word[1] ^ y->word[6]; \
    z->word[2] = x->word[2] ^ y->word[5]; \
    z->word[3] = x->word[3] ^ y->word[4]; \
    z->word[4] = x->word[4] ^ y->word[3]; \
    z->word[5] = x->word[5] ^ y->word[2]; \
    z->word[6] = x->word[6] ^ y->word[1]; \
    z->word[7] = x->word[7] ^ y->word[0]; \
}

static const union uint512_u buffer512  = {{ 512, 0, 0, 0, 0, 0, 0, 0 }};
static const union uint512_u buffer0    = {{ 0, 0, 0, 0, 0, 0, 0, 0 }};

static const union uint512_u C[12] = {
    {{    
          0xb1085bda1ecadae9,
          0xebcb2f81c0657c1f,
          0x2f6a76432e45d016,
          0x714eb88d7585c4fc,
          0x4b7ce09192676901,
          0xa2422a08a460d315,
          0x05767436cc744d23,
          0xdd806559f2a64507
    }},
    {{
          0x6fa3b58aa99d2f1a,
          0x4fe39d460f70b5d7,
          0xf3feea720a232b98,
          0x61d55e0f16b50131,
          0x9ab5176b12d69958,
          0x5cb561c2db0aa7ca,
          0x55dda21bd7cbcd56,
          0xe679047021b19bb7
    }},
    {{
          0xf574dcac2bce2fc7,
          0x0a39fc286a3d8435,
          0x06f15e5f529c1f8b,
          0xf2ea7514b1297b7b,
          0xd3e20fe490359eb1,
          0xc1c93a376062db09,
          0xc2b6f443867adb31,
          0x991e96f50aba0ab2
    }},
    {{
          0xef1fdfb3e81566d2,
          0xf948e1a05d71e4dd,
          0x488e857e335c3c7d,
          0x9d721cad685e353f,
          0xa9d72c82ed03d675,
          0xd8b71333935203be,
          0x3453eaa193e837f1,
          0x220cbebc84e3d12e
    }},
    {{
          0x4bea6bacad474799,
          0x9a3f410c6ca92363,
          0x7f151c1f1686104a,
          0x359e35d7800fffbd,
          0xbfcd1747253af5a3,
          0xdfff00b723271a16,
          0x7a56a27ea9ea63f5,
          0x601758fd7c6cfe57
    }},
    {{
          0xae4faeae1d3ad3d9,
          0x6fa4c33b7a3039c0,
          0x2d66c4f95142a46c,
          0x187f9ab49af08ec6,
          0xcffaa6b71c9ab7b4,
          0x0af21f66c2bec6b6,
          0xbf71c57236904f35,
          0xfa68407a46647d6e
    }},
    {{
          0xf4c70e16eeaac5ec,
          0x51ac86febf240954,
          0x399ec6c7e6bf87c9,
          0xd3473e33197a93c9,
          0x0992abc52d822c37,
          0x06476983284a0504,
          0x3517454ca23c4af3,
          0x8886564d3a14d493
    }},
    {{
          0x9b1f5b424d93c9a7,
          0x03e7aa020c6e4141,
          0x4eb7f8719c36de1e,
          0x89b4443b4ddbc49a,
          0xf4892bcb929b0690,
          0x69d18d2bd1a5c42f,
          0x36acc2355951a8d9,
          0xa47f0dd4bf02e71e
    }},
    {{
          0x378f5a541631229b,
          0x944c9ad8ec165fde,
          0x3a7d3a1b25894224,
          0x3cd955b7e00d0984,
          0x800a440bdbb2ceb1,
          0x7b2b8a9aa6079c54,
          0x0e38dc92cb1f2a60,
          0x7261445183235adb
    }},
    {{
          0xabbedea680056f52,
          0x382ae548b2e4f3f3,
          0x8941e71cff8a78db,
          0x1fffe18a1b336103,
          0x9fe76702af69334b,
          0x7a1e6c303b7652f4,
          0x3698fad1153bb6c3,
          0x74b4c7fb98459ced
    }},
    {{
          0x7bcd9ed0efc889fb,
          0x3002c6cd635afe94,
          0xd8fa6bbbebab0761,
          0x2001802114846679,
          0x8a1d71efea48b9ca,
          0xefbacd1d7d476e98,
          0xdea2594ac06fd85d,
          0x6bcaa4cd81f32d1b
    }},
    {{
          0x378ee767f11631ba,
          0xd21380b00449b17a,
          0xcda43c32bcdf1d77,
          0xf82012d430219f9b,
          0x5d80ef9d1891cc86,
          0xe71da4aa88e12852,
          0xfaf417d5d9b21b99,
          0x48bc924af11bd720
    }}
};

static const uint64_t A[129] = { 
    0x8e20faa72ba0b470, 0x47107ddd9b505a38, 0xad08b0e0c3282d1c,
    0xd8045870ef14980e, 0x6c022c38f90a4c07, 0x3601161cf205268d,
    0x1b8e0b0e798c13c8, 0x83478b07b2468764, 0xa011d380818e8f40,
    0x5086e740ce47c920, 0x2843fd2067adea10, 0x14aff010bdd87508,
    0x0ad97808d06cb404, 0x05e23c0468365a02, 0x8c711e02341b2d01,
    0x46b60f011a83988e, 0x90dab52a387ae76f, 0x486dd4151c3dfdb9,
    0x24b86a840e90f0d2, 0x125c354207487869, 0x092e94218d243cba,
    0x8a174a9ec8121e5d, 0x4585254f64090fa0, 0xaccc9ca9328a8950,
    0x9d4df05d5f661451, 0xc0a878a0a1330aa6, 0x60543c50de970553,
    0x302a1e286fc58ca7, 0x18150f14b9ec46dd, 0x0c84890ad27623e0,
    0x0642ca05693b9f70, 0x0321658cba93c138, 0x86275df09ce8aaa8,
    0x439da0784e745554, 0xafc0503c273aa42a, 0xd960281e9d1d5215,
    0xe230140fc0802984, 0x71180a8960409a42, 0xb60c05ca30204d21,
    0x5b068c651810a89e, 0x456c34887a3805b9, 0xac361a443d1c8cd2,
    0x561b0d22900e4669, 0x2b838811480723ba, 0x9bcf4486248d9f5d,
    0xc3e9224312c8c1a0, 0xeffa11af0964ee50, 0xf97d86d98a327728,
    0xe4fa2054a80b329c, 0x727d102a548b194e, 0x39b008152acb8227,
    0x9258048415eb419d, 0x492c024284fbaec0, 0xaa16012142f35760,
    0x550b8e9e21f7a530, 0xa48b474f9ef5dc18, 0x70a6a56e2440598e,
    0x3853dc371220a247, 0x1ca76e95091051ad, 0x0edd37c48a08a6d8,
    0x07e095624504536c, 0x8d70c431ac02a736, 0xc83862965601dd1b,
    0x641c314b2b8ee083, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
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

static uint64_t Ax[8][256];

static const uint8_t Tau[64] = {
    0,   8,  16,  24,  32,  40,  48,  56, 
    1,   9,  17,  25,  33,  41,  49,  57, 
    2,  10,  18,  26,  34,  42,  50,  58, 
    3,  11,  19,  27,  35,  43,  51,  59, 
    4,  12,  20,  28,  36,  44,  52,  60, 
    5,  13,  21,  29,  37,  45,  53,  61, 
    6,  14,  22,  30,  38,  46,  54,  62, 
    7,  15,  23,  31,  39,  47,  55,  63
};

static const uint64_t Pi[256] = {
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
    void *p;
    long offset;

    if ((p = malloc(size)) == NULL)
        err(EX_OSERR, NULL);

    /* Ensure p is on a 64-bit boundary. */ 
    if ((offset = (long) p & 7L))
        p += 8 - offset;

    return p;
}

void 
init(const uint32_t digest_size, GOST3411Context *CTX)
{
    uint8_t i, j;
    uint16_t b;
    Ai_t idx1, idx2;

    CTX->N = memalloc(sizeof uint512_u);
    CTX->h = memalloc(sizeof uint512_u);
    CTX->hash = memalloc(sizeof uint512_u);
    CTX->Sigma = memalloc(sizeof uint512_u);
    CTX->buffer = memalloc(sizeof uint512_u);
    CTX->hexdigest = memalloc(129);

    *(CTX->N) = buffer0;
    *(CTX->h) = buffer0;
    *(CTX->hash) = buffer0;
    *(CTX->Sigma) = buffer0;
    *(CTX->buffer) = buffer0;
    CTX->digest_size = digest_size;
    memset(CTX->hexdigest, 0, 1);

    for (i = 0; i < 8; i++)
    {
        if (digest_size == 256)
            CTX->h->word[i] = 0x0101010101010101UL;
        else
            CTX->h->word[i] = 0UL;
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
}

static void
pad(union uint512_u *data)
{
    uint8_t i;

    i = 64;
    do
    {
        i--;
        if (data->word[i >> 3] == 0)
        {
            if (i >> 3 == 0)
            {
                data->byte[0] = 0x01;
                break;
            }

            i = (i >> 3) * 8;
            continue;
        }
        if (data->byte[i])
        {
            data->byte[++i] = 0x01;
            break;
        }
    }
    while (i > 0);
}

static inline void 
LPS(union uint512_u *data)
{
    uint8_t i, j, k;
    union uint512_u buf;

    /* Substitution */
    for (i = 0; i < 64; i++)
        data->byte[i] = Pi[data->byte[i]];

    /* Permutaion */
    for (i = 0; i < 64; i++)
        buf.byte[i] = data->byte[Tau[i]];

    (*data) = buf;

    buf = buffer0;

    /* Linear transformation */
    for (i = 0; i < 8; i++)
    {
        j = i << 3;
        for (k = 0; k < 8; k++)
            buf.word[i] ^= Ax[k][data->byte[j + k]];
    }

    (*data) = buf;
}

static inline void
K(const uint8_t i, union uint512_u *Ki)
{
    XR(Ki, (&C[i - 2]), Ki);
    LPS(Ki);
}

static inline void
E(const union uint512_u *Key, const union uint512_u *m, union uint512_u *data)
{
    uint8_t i;
    union uint512_u Ki;

    Ki = (*Key);
    X((&Ki), m, data);
    LPS(data);
    for (i = 2; i < 13; i++)
    {
        K(i, &Ki);
        X((&Ki), data, data);
        LPS(data);
    }

    K(13, &Ki);
    X((&Ki), data, data);
}

static inline void
add512(const union uint512_u *x, const union uint512_u *y, union uint512_u *r)
{
    uint64_t CF;
    int8_t i;

    CF = 0;
    for (i = 0; i < 8; i++)
    {
        r->word[i] = x->word[i] + y->word[i] + CF;
        /* Actually it is sufficient to compare *r with ANY argument. However,
         * one argument of add512() call MAY point to *r at the same time.
         * Consider add512(x, y, x) or ad512(x, y, y). We avoid confusion by
         * comparing *r with both arguments. Instead of *y, assume that *x
         * probably points to *r, so *y goes first.
         */
        if ( (r->word[i] < y->word[i]) || 
             (r->word[i] < x->word[i]) )
            CF = 1;
        else
            CF = 0;
    }
}

static inline void
g(const union uint512_u *N, const union uint512_u *h, const union uint512_u *m, 
        union uint512_u *data)
{
    X(h, N, data);
    LPS(data);
    E(data, m, data);
    X(data, h, data);
    X(data, m, data);
}

static void
round2(GOST3411Context *CTX)
{
    g(CTX->N, CTX->h, CTX->buffer, CTX->hash);

    (*CTX->h) = (*CTX->hash);

    add512(CTX->N, &buffer512, CTX->N);
    add512(CTX->Sigma, CTX->buffer, CTX->Sigma);
}

static void
round3(GOST3411Context *CTX)
{
    union uint512_u buf;

    buf = buffer0;
    memcpy(&buf, CTX->buffer, CTX->bufsize);
    (*CTX->buffer) = buf;

    buf = buffer0;
    buf.word[0] = CTX->bufsize * 8;

    pad(CTX->buffer);

    g(CTX->N, CTX->h, CTX->buffer, CTX->hash);
    (*CTX->h) = (*CTX->hash);

    add512(CTX->N, &buf, CTX->N);
    add512(CTX->Sigma, CTX->buffer, CTX->Sigma);
        
    g(&buffer0, CTX->h, CTX->N, CTX->hash); 
    (*CTX->h) = (*CTX->hash);
    g(&buffer0, CTX->h, CTX->Sigma, CTX->hash);
}

void
update(GOST3411Context *CTX, const void *data, size_t len)
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
    int8_t i, j;
    char *buf;

    round3(CTX);
    CTX->bufsize = 0;

    buf = memalloc(17);

    if (CTX->digest_size == 256)
        j = 4;
    else
        j = 0;

    i = 7;
    while (i >= j)
    {
        snprintf(buf, 17, "%.16lx", CTX->hash->word[i]);
        strncat(CTX->hexdigest, buf, 16);
        i--;
    }

    free(buf);
}
