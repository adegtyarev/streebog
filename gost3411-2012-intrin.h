#include <xmmintrin.h>
#include <emmintrin.h>

#define X64(x, y, z) { \
    z->QWORD[0] = x->QWORD[0] ^ y->QWORD[0]; \
    z->QWORD[1] = x->QWORD[1] ^ y->QWORD[1]; \
    z->QWORD[2] = x->QWORD[2] ^ y->QWORD[2]; \
    z->QWORD[3] = x->QWORD[3] ^ y->QWORD[3]; \
    z->QWORD[4] = x->QWORD[4] ^ y->QWORD[4]; \
    z->QWORD[5] = x->QWORD[5] ^ y->QWORD[5]; \
    z->QWORD[6] = x->QWORD[6] ^ y->QWORD[6]; \
    z->QWORD[7] = x->QWORD[7] ^ y->QWORD[7]; \
}

#define X32(x, y, z) {\
    __m128i *pz; \
    const __m128i *px = (const __m128i *) &x[0]; \
    const __m128i *py = (const __m128i *) &y[0]; \
    pz = (__m128i *) z; \
    _mm_store_si128(&pz[0], _mm_xor_si128(px[0], py[0])); \
    _mm_store_si128(&pz[1], _mm_xor_si128(px[1], py[1])); \
    _mm_store_si128(&pz[2], _mm_xor_si128(px[2], py[2])); \
    _mm_store_si128(&pz[3], _mm_xor_si128(px[3], py[3])); \
}

#define XLOADR(x, y, xmm0, xmm1, xmm2, xmm3) { \
    __m128i *px = (__m128i *) &x[0]; \
    __m128i *py = (__m128i *) &y[0]; \
    xmm0 = _mm_xor_si128(px[0], py[0]); \
    xmm1 = _mm_xor_si128(px[1], py[1]); \
    xmm2 = _mm_xor_si128(px[2], py[2]); \
    xmm3 = _mm_xor_si128(px[3], py[3]); \
}

#define LOAD(P, xmm0, xmm1, xmm2, xmm3) { \
    __m128i *__m128p = (__m128i *) &P[0]; \
    xmm0 = _mm_load_si128(&__m128p[0]); \
    xmm1 = _mm_load_si128(&__m128p[1]); \
    xmm2 = _mm_load_si128(&__m128p[2]); \
    xmm3 = _mm_load_si128(&__m128p[3]); \
}

#define TRANSPOSE(xmm0, xmm1, xmm2, xmm3) {\
    __m128i txm0, txm1, txm2, txm3; \
    txm0 = _mm_unpacklo_epi8(xmm0, xmm1); \
    txm1 = _mm_unpackhi_epi8(xmm0, xmm1); \
    txm2 = _mm_unpacklo_epi8(xmm2, xmm3); \
    txm3 = _mm_unpackhi_epi8(xmm2, xmm3); \
    \
    xmm0 = _mm_unpacklo_epi8(txm0, txm1); \
    xmm1 = _mm_unpackhi_epi8(txm0, txm1); \
    xmm2 = _mm_unpacklo_epi8(txm2, txm3); \
    xmm3 = _mm_unpackhi_epi8(txm2, txm3); \
    \
    txm1 = _mm_unpackhi_epi32(xmm0, xmm2); \
    xmm0 = _mm_unpacklo_epi32(xmm0, xmm2); \
    xmm2 = _mm_unpacklo_epi32(xmm1, xmm3); \
    xmm3 = _mm_unpackhi_epi32(xmm1, xmm3); \
    xmm1 = txm1; \
}

#define UNLOAD(P, xmm0, xmm1, xmm2, xmm3) { \
    __m128i *__m128p = (__m128i *) &P[0]; \
    _mm_store_si128(&__m128p[0], xmm0); \
    _mm_store_si128(&__m128p[1], xmm1); \
    _mm_store_si128(&__m128p[2], xmm2); \
    _mm_store_si128(&__m128p[3], xmm3); \
}

#define XTRANSPOSE(x, y, data) {\
    __m128i xmm0, xmm1, xmm2, xmm3; \
    const __m128i *px = (const __m128i *) &x[0]; \
    const __m128i *py = (const __m128i *) &y[0]; \
    xmm0 = _mm_xor_si128(px[0], py[0]); \
    xmm1 = _mm_xor_si128(px[1], py[1]); \
    xmm2 = _mm_xor_si128(px[2], py[2]); \
    xmm3 = _mm_xor_si128(px[3], py[3]); \
    TRANSPOSE(xmm0, xmm1, xmm2, xmm3); \
    UNLOAD(data, xmm0, xmm1, xmm2, xmm3); \
}

#define XLPS32(x, y, data) { \
    unsigned int xi; \
    uint8_t *p; \
    union uint512_u buf  __attribute__((aligned(16))); \
    XTRANSPOSE(x, y, (&buf)); \
    p = (uint8_t *) &buf; \
    for (xi = 0; xi < 8; xi++) \
    { \
        data->QWORD[xi]  = Ax[0][Pi[*(p++)]]; \
        data->QWORD[xi] ^= Ax[1][Pi[*(p++)]]; \
        data->QWORD[xi] ^= Ax[2][Pi[*(p++)]]; \
        data->QWORD[xi] ^= Ax[3][Pi[*(p++)]]; \
        data->QWORD[xi] ^= Ax[4][Pi[*(p++)]]; \
        data->QWORD[xi] ^= Ax[5][Pi[*(p++)]]; \
        data->QWORD[xi] ^= Ax[6][Pi[*(p++)]]; \
        data->QWORD[xi] ^= Ax[7][Pi[*(p++)]]; \
    } \
}

#define XLPS64(x, y, data) { \
    register unsigned long long r0, r1, r2, r3, r4, r5, r6, r7; \
    uint8_t i; \
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
