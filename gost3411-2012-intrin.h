/*
 * $Id$
 */

#include <emmintrin.h>
#include <mmintrin.h>

#define XLOAD(x, y, xmm0, xmm1, xmm2, xmm3) { \
    const __m128i *px = (const __m128i *) &x[0]; \
    const __m128i *py = (const __m128i *) &y[0]; \
    xmm0 = _mm_xor_si128(px[0], py[0]); \
    xmm1 = _mm_xor_si128(px[1], py[1]); \
    xmm2 = _mm_xor_si128(px[2], py[2]); \
    xmm3 = _mm_xor_si128(px[3], py[3]); \
}

#define UNLOAD(P, xmm0, xmm1, xmm2, xmm3) { \
    __m128i *__m128p = (__m128i *) &P[0]; \
    _mm_store_si128(&__m128p[0], xmm0); \
    _mm_store_si128(&__m128p[1], xmm1); \
    _mm_store_si128(&__m128p[2], xmm2); \
    _mm_store_si128(&__m128p[3], xmm3); \
}

#define TRANSPOSE(xmm0, xmm1, xmm2, xmm3) { \
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

#define XTRANSPOSE(x, y, z) { \
    __m128i xmm0, xmm1, xmm2, xmm3; \
    XLOAD(x, y, xmm0, xmm1, xmm2, xmm3); \
    TRANSPOSE(xmm0, xmm1, xmm2, xmm3); \
    UNLOAD(z, xmm0, xmm1, xmm2, xmm3); \
}

#define X128(x, y, z) { \
    __m128i xmm0, xmm1, xmm2, xmm3; \
    XLOAD(x, y, xmm0, xmm1, xmm2, xmm3); \
    UNLOAD(  z, xmm0, xmm1, xmm2, xmm3); \
}

#define _mm_cvtsi64_m64(v) (__m64) v
#define _mm_xor_64(mm0, mm1) _mm_xor_si64(mm0, _mm_cvtsi64_m64(mm1))

#define XLPS32(x, y, data) { \
    unsigned int xi; \
    uint8_t *p; \
    union uint512_u buf  __attribute__((aligned(16))); \
    __m64 mm0; \
    XTRANSPOSE(x, y, (&buf)); \
    p = (uint8_t *) &buf; \
    for (xi = 0; xi < 8; xi++) \
    { \
        mm0 = _mm_cvtsi64_m64(Ax[0][Pi[*(p++)]]); \
        mm0 = _mm_xor_64(mm0, Ax[1][Pi[*(p++)]]); \
        mm0 = _mm_xor_64(mm0, Ax[2][Pi[*(p++)]]); \
        mm0 = _mm_xor_64(mm0, Ax[3][Pi[*(p++)]]); \
        mm0 = _mm_xor_64(mm0, Ax[4][Pi[*(p++)]]); \
        mm0 = _mm_xor_64(mm0, Ax[5][Pi[*(p++)]]); \
        mm0 = _mm_xor_64(mm0, Ax[6][Pi[*(p++)]]); \
        mm0 = _mm_xor_64(mm0, Ax[7][Pi[*(p++)]]); \
        data->QWORD[xi] = (unsigned long long) mm0; \
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
