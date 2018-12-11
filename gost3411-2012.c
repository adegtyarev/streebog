/* 
 * Copyright (c) 2013, Alexey Degtyarev <alexey@renatasystems.org>. 
 * All rights reserved.
 *
 * GOST 34.11-2012 hash function with 512/256 bits digest.
 *
 * $Id$
 */

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "gost3411-2012-core.h"

/* For benchmarking */
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>

#define READ_BUFFER_SIZE 65536

#define TEST_BLOCK_LEN 8192

#ifdef __GOST3411_HAS_SSE2__
#define TEST_BLOCK_COUNT 50000
#else
#define TEST_BLOCK_COUNT 10000
#endif

#define DEFAULT_DIGEST_SIZE 512
#define ALGNAME "GOST R 34.11-2012"
#define VERSION "0.13"

GOST34112012Context *CTX;

unsigned char digest[64];
unsigned char hexdigest[129];
unsigned int digest_size = DEFAULT_DIGEST_SIZE;

const union uint512_u GOSTTestInput = {
#ifndef __GOST3411_BIG_ENDIAN__
    {
        0x3736353433323130ULL,
        0x3534333231303938ULL,
        0x3332313039383736ULL,
        0x3130393837363534ULL,
        0x3938373635343332ULL,
        0x3736353433323130ULL,
        0x3534333231303938ULL,
        0x0032313039383736ULL
    }
#else
    {
        0x3031323334353637ULL,
        0x3839303132333435ULL,
        0x3637383930313233ULL,
        0x3435363738393031ULL,
        0x3233343536373839ULL,
        0x3031323334353637ULL,
        0x3839303132333435ULL,
        0x3637383930313200ULL
    }
#endif
};

static void
usage_short(void)
{
    fprintf(stderr, "Usage: [-25bhvqrte] [-s string] [files ...]\n");
    exit(255);
}

static void
usage(void)
{
    fprintf(stdout, "The program outputs GOST R 34.11-2012 hash digest in hexadecimal format.\n");
    fprintf(stdout, "Each file listed on the command line is processed and hash is printed\n");
    fprintf(stdout, "for each one.  Stdin is read as input when executed without arguments.\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Usage: gost3411-2012 [-25bhvqrte] [-s string] [files ...]\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "   -2          Output 256-bit digest.\n");
    fprintf(stdout, "   -5          Output 512-bit digest (default).\n");
    fprintf(stdout, "   -t          Testing mode to produce hash of example\n");
    fprintf(stdout, "               messages defined in standard.\n");
    fprintf(stdout, "   -b          Benchmark mode (to see how fast or slow\n");
    fprintf(stdout, "               this implementation).\n");
    fprintf(stdout, "   -s string   Print a digest of the given string.\n");
    fprintf(stdout, "   -r          Reverses the format of the output.\n");
    fprintf(stdout, "               This helps with visual diffs.\n");
    fprintf(stdout, "   -q          Quiet mode - only the digest is printed\n");
    fprintf(stdout, "               out.\n");
    fprintf(stdout, "   -e          Switch endianness when printing out\n");
    fprintf(stdout, "               resulting hash.  Default: least significant\n");
    fprintf(stdout, "               first.  With this options set all bytes in\n");
    fprintf(stdout, "               resulting hash are printed in reverse\n");
    fprintf(stdout, "               order, more precisely, most significant\n");
    fprintf(stdout, "               first.\n");
    fprintf(stdout, "   -v          Print version info and exit.\n");
    fprintf(stdout, "   -h          Print this help and exit.\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Report bugs to <alexey@renatasystems.org>\n");
    exit(0);
}

static void
version(void)
{
    fprintf(stdout, "gost3411-2012 %s\n", VERSION);
    fprintf(stdout, "\n");
    fprintf(stdout, "Written by Alexey Degtyarev.\n");
    exit(0);
}

static void *
memalloc(const size_t size)
{
    void *p;

    /* Ensure p is on a 64-bit boundary. */ 
    if (posix_memalign(&p, (size_t) 64, size))
        err(EX_OSERR, NULL);

    return p;
}

static void
reverse_order(unsigned char *in, size_t len)
{
    unsigned char c;
    unsigned int i, j;

    for (i = 0, j = (unsigned int) len - 1; i < j; i++, j--)
    {
        c = in[i];
        in[i] = in[j];
        in[j] = c;
    }
}

static void
convert_to_hex(unsigned char *in, unsigned char *out, size_t len,
        const unsigned int eflag)
{
    unsigned int i;
    char ch[3];

    if (len > 64) len = 64;

    memset(out, 0, 129);

    /* eflag is set when little-endian output requested */
    if (eflag) reverse_order(in, len);

    for (i = 0; i < len; i++)
    {
        sprintf(ch, "%02x", (unsigned char) in[i]);
        memcpy(&out[i * 2], ch, 2);
    }
}

static void
onfile(FILE *file)
{
    unsigned char *buffer;
    size_t len;

    GOST34112012Init(CTX, digest_size);

    buffer = memalloc((size_t) READ_BUFFER_SIZE);

    while ((len = fread(buffer, (size_t) 1, (size_t) READ_BUFFER_SIZE, file)))
        GOST34112012Update(CTX, buffer, len);

    if (ferror(file))
        err(EX_IOERR, NULL);

    free(buffer);

    GOST34112012Final(CTX, &digest[0]);
}

static void
onstring(const unsigned char *string)
{
    unsigned char *buf __attribute__((aligned(16)));
    size_t size;

    GOST34112012Init(CTX, digest_size);

    size = strnlen((const char *) string, (size_t) 4096);
    buf = memalloc(size);
    memcpy(buf, string, size);

    GOST34112012Update(CTX, buf, size);

    GOST34112012Final(CTX, &digest[0]);

}

static void
testing(const unsigned int eflag)
{
    printf("M1: 012345678901234567890123456789012345678901234567890123456789012\n");

    GOST34112012Init(CTX, 512);

    memcpy(CTX->buffer, &GOSTTestInput, sizeof uint512_u);
    CTX->bufsize = 63;

    GOST34112012Final(CTX, &digest[0]);

    convert_to_hex(&digest[0], &hexdigest[0], 64, eflag);
    printf("%s 512 bit digest (M1): 0x%s\n", ALGNAME, hexdigest);

    GOST34112012Cleanup(CTX);

    GOST34112012Init(CTX, 256);

    memcpy(CTX->buffer, &GOSTTestInput, sizeof uint512_u);
    CTX->bufsize = 63;

    GOST34112012Final(CTX, &digest[0]);

    convert_to_hex(&digest[0], &hexdigest[0], 32, eflag);
    printf("%s 256 bit digest (M1): 0x%s\n", ALGNAME, hexdigest);

    GOST34112012Cleanup(CTX);

    exit(EXIT_SUCCESS);
}

static void
benchmark(const unsigned int eflag)
{
    struct rusage before, after;
    struct timeval total;
    float seconds;
    unsigned char block[TEST_BLOCK_LEN] __attribute__((aligned(16)));
    unsigned int i;

    printf("%s timing benchmark.\n", ALGNAME);
    printf("Digesting %d %d-byte blocks with 512 bits digest...\n",
        TEST_BLOCK_COUNT, TEST_BLOCK_LEN);
    fflush(stdout);

    for (i = 0; i < TEST_BLOCK_LEN; i++)
        block[i] = (unsigned char) (i & 0xff);

    getrusage(RUSAGE_SELF, &before);

    GOST34112012Init(CTX, 512);
    for (i = 0; i < TEST_BLOCK_COUNT; i++)
        GOST34112012Update(CTX, block, (size_t) TEST_BLOCK_LEN);
    GOST34112012Final(CTX, digest);

    getrusage(RUSAGE_SELF, &after);
    timersub(&after.ru_utime, &before.ru_utime, &total);
    seconds = (float) total.tv_sec + (float) total.tv_usec / 1000000;

    convert_to_hex(digest, hexdigest, 64, eflag);

    printf("Digest = %s", hexdigest);
    printf("\nTime = %f seconds\n", seconds);
    printf("Speed = %.2f bytes/second\n",
        (float) TEST_BLOCK_LEN * (float) TEST_BLOCK_COUNT / seconds);

    exit(EXIT_SUCCESS);
}

static void 
shutdown(void)
{
    if (CTX != NULL)
        GOST34112012Cleanup(CTX);
}

#ifdef SUPERCOP
#include "crypto_hash.h"
int
crypto_hash(unsigned char *out, const unsigned char *in,
        unsigned long long inlen)
{
    CTX = memalloc(sizeof(GOST34112012Context));

    GOST34112012Init(CTX, 512);
    GOST34112012Update(CTX, in, (size_t) inlen);
    GOST34112012Final(CTX, out);

    return 0;
}
#else
int
main(int argc, char *argv[])
{
    int ch; 
    unsigned char uflag, qflag, rflag, eflag;
    unsigned char excode;
    FILE *f;

    excode = EXIT_SUCCESS;
    atexit(shutdown);

    CTX = memalloc(sizeof(GOST34112012Context));

    qflag = 0;
    rflag = 0;
    uflag = 0;
    eflag = 0;

    while ((ch = getopt(argc, argv, "25bhvqrs:te")) != -1)
    {
        switch (ch)
        {
            case 'b':
                benchmark(eflag);
                break;
            case '2':
                digest_size = 256;
                break;
            case '5':
                digest_size = 512;
                break;
            case 'q':
                qflag = 1;
                break;
            case 's':
                onstring((unsigned char *) optarg);

                if (digest_size == 256)
                    convert_to_hex(digest, hexdigest, 32, eflag);
                else
                    convert_to_hex(digest, hexdigest, 64, eflag);

                if (qflag)
                    printf("%s\n", hexdigest);
                else if (rflag)
                    printf("%s \"%s\"\n", hexdigest, optarg);
                else
                    printf("%s (\"%s\") = %s\n", ALGNAME, optarg,
                            hexdigest);
                uflag = 1;
                break;
            case 'r':
                rflag = 1;
                break;
            case 't':
                testing(eflag);
                break;
            case 'e':
                eflag = 1;
                break;
            case 'h':
                usage();
                break;
            case 'v':
                version();
                break;
            default:
                usage_short();
        }
    }

    argc -= optind;
    argv += optind;

    if (*argv)
    {
        do
        {
            if ((f = fopen(*argv, "rb")) == NULL)
            {
                warn("%s", *argv);
                excode = EX_OSFILE;
                continue;
            }
            onfile(f);
            fclose(f);
            uflag = 1;

            if (digest_size == 256)
                convert_to_hex(digest, hexdigest, 32, eflag);
            else
                convert_to_hex(digest, hexdigest, 64, eflag);

            if (qflag)
                printf("%s\n", hexdigest);
            else if (rflag)
                printf("%s \"%s\"\n", hexdigest, *argv);
            else
                printf("%s (%s) = %s\n", ALGNAME, *argv, hexdigest);
        }
        while (*++argv);
    }
    else if (!uflag)
    {
        onfile(stdin);

        if (digest_size == 256)
            convert_to_hex(digest, hexdigest, 32, eflag);
        else
            convert_to_hex(digest, hexdigest, 64, eflag);

        printf("%s\n", hexdigest);

        uflag = 1;
    }

    if (!uflag)
        usage();

    return excode;
}
#endif
