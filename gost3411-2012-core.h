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

#define DEFAULT_DIGEST_SIZE 512
#define ALGNAME "GOST R 34.11-2012"

typedef struct uint512_t
{
    uint64_t c[8];
} uint512_t;

union uint512_u
{
    uint64_t word[8];
    uint8_t byte[64];
} uint512_u;

typedef struct Ai_t
{
    uint8_t i[4];
} Ai_t;

typedef struct GOST3411Context
{
    union uint512_u *buffer;
    union uint512_u *hash;
    union uint512_u *h;
    union uint512_u *N;
    union uint512_u *Sigma;
    size_t bufsize;
    uint32_t digest_size;
    char *hexdigest;
} GOST3411Context;

void *memalloc(const size_t size);
void final(GOST3411Context *CTX);
void init(const uint32_t digest_size, GOST3411Context *CTX);
void update(GOST3411Context *CTX, const void *data, uint32_t len);
