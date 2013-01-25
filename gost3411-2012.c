/* 
 * GOST 34.11-2012 hash function with 512/256 bits digest.
 *
 * $Id$
 */

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

GOST3411Context *CTX;

unsigned int digest_size = DEFAULT_DIGEST_SIZE;

static void
usage(void)
{
    fprintf(stderr, "usage: [-25bhqrt] [-s string] [files ...]\n");
    exit(1);
}

static void
onfile(FILE *file)
{
    unsigned char *buffer;
    size_t len;

    CTX = GOST3411Init(digest_size);

    buffer = memalloc((size_t) READ_BUFFER_SIZE + 7);

    while ((len = fread(buffer, (size_t) 1, (size_t) READ_BUFFER_SIZE, file)))
        GOST3411Update(CTX, buffer, len);

    if (ferror(file))
        err(EX_IOERR, NULL);

    free(buffer);

    GOST3411Final(CTX);
}

static void
onstring(const unsigned char *string)
{
    CTX = GOST3411Init(digest_size);

    GOST3411Update(CTX, string, strlen((const char *) string));

    GOST3411Final(CTX);
}

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
testing(void)
{
    CTX = GOST3411Init(512);

    memcpy(CTX->buffer, &GOSTTestInput, sizeof uint512_u);
    CTX->bufsize = 63;

    printf("M1: 0x%.16llx%.16llx%.16llx%.16llx%.16llx%.16llx%.16llx%.16llx\n",
           CTX->buffer->QWORD[7], CTX->buffer->QWORD[6], CTX->buffer->QWORD[5],
           CTX->buffer->QWORD[4], CTX->buffer->QWORD[3], CTX->buffer->QWORD[2],
           CTX->buffer->QWORD[1], CTX->buffer->QWORD[0]);

    GOST3411Final(CTX);
    printf("%s 512 bit digest (M1): 0x%s\n", ALGNAME, CTX->hexdigest);

    GOST3411Destroy(CTX);

    CTX = GOST3411Init(256);

    memcpy(CTX->buffer, &GOSTTestInput, sizeof uint512_u);
    CTX->bufsize = 63;

    GOST3411Final(CTX);
    printf("%s 256 bit digest (M1): 0x%s\n", ALGNAME, CTX->hexdigest);

    /* This guy causes double free on Linux :-? 
    GOST3411Destroy(CTX);
    */

    exit(EXIT_SUCCESS);
}

static void
benchmark(void)
{
    struct rusage before, after;
    struct timeval total;
    float seconds;
    unsigned char block[TEST_BLOCK_LEN];
    unsigned int i;

    printf("%s timing benchmark.\n", ALGNAME);
    printf("Digesting %d %d-byte blocks with 512 bits digest...\n",
        TEST_BLOCK_COUNT, TEST_BLOCK_LEN);
    fflush(stdout);

    for (i = 0; i < TEST_BLOCK_LEN; i++)
        block[i] = (unsigned char) (i & 0xff);

    getrusage(RUSAGE_SELF, &before);

    CTX = GOST3411Init(512);
    for (i = 0; i < TEST_BLOCK_COUNT; i++)
        GOST3411Update(CTX, block, (size_t) TEST_BLOCK_LEN);
    GOST3411Final(CTX);

    getrusage(RUSAGE_SELF, &after);
    timersub(&after.ru_utime, &before.ru_utime, &total);
    seconds = (float) total.tv_sec + (float) total.tv_usec / 1000000;

    printf("Digest = %s", CTX->hexdigest);
    printf("\nTime = %f seconds\n", seconds);
    printf("Speed = %.2f bytes/second\n",
        (float) TEST_BLOCK_LEN * (float) TEST_BLOCK_COUNT / seconds);

    exit(EXIT_SUCCESS);
}

static void 
shutdown(void)
{
    if (CTX != NULL)
        GOST3411Destroy(CTX);
}

#ifdef SUPERCOP
#include "crypto_hash.h"
int
crypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
    CTX = GOST3411Init(512);

    GOST3411Update(CTX, in, (size_t) inlen);
    GOST3411Final(CTX);

    memcpy(out, CTX->hexdigest, 64);

    return 0;
}
#else
int
main(int argc, char *argv[])
{
    int ch; 
    char uflag, qflag, rflag, excode;
    FILE *f;

    excode = EXIT_SUCCESS;
    atexit(shutdown);

    qflag = 0;
    rflag = 0;
    uflag = 0;

    while ((ch = getopt(argc, argv, "25bhqrs:t")) != -1)
    {
        switch (ch)
        {
            case 'b':
                benchmark();
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
                if (qflag)
                    printf("%s\n", CTX->hexdigest);
                else if (rflag)
                    printf("%s \"%s\"\n", CTX->hexdigest, optarg);
                else
                    printf("%s (\"%s\") = %s\n", ALGNAME, optarg,
                            CTX->hexdigest);
                uflag = 1;
                break;
            case 'r':
                rflag = 1;
                break;
            case 't':
                testing();
                break;
            case 'h':
            default:
                usage();
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
            if (qflag)
                printf("%s\n", CTX->hexdigest);
            else if (rflag)
                printf("%s \"%s\"\n", CTX->hexdigest, *argv);
            else
                printf("%s (%s) = %s\n", ALGNAME, *argv, CTX->hexdigest);
        }
        while (*++argv);
    }
    else if (!uflag)
    {
        onfile(stdin);
        printf("%s\n", CTX->hexdigest);
        uflag = 1;
    }

    if (! uflag)
        usage();

    return excode;
}
#endif
