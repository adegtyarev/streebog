GOST R 34.11-2012 hash function with 512/256 bit digest
=======================================================

[![Build Status](https://travis-ci.org/adegtyarev/streebog.svg?branch=master)](https://travis-ci.org/adegtyarev/streebog)

This is portable implementation of the GOST R 34.11-2012 hash function.
The standard for this hash function developed by the Center for
Information Protection and Special Communications of the Federal
Security Service of the Russian Federation with participation of the
Open joint-stock company "Information Technologies and Communication
Systems" (InfoTeCS JSC).

The standard published as [RFC 6986](https://tools.ietf.org/html/rfc6986).


Build requirements
------------------
* GCC, Clang or ICC compiler supporting 64-bit integers.

* GNU make (or any compatible make).


Compile and install
-------------------
The software is smart enough to detect the most suitable configuration
for running hardware and software platform.  In almost all cases it is
sufficient to run `make` on top of the source directory:

    # make

This will configure and compile a binary program file named
`gost3411-2012`.


Usage instructions
------------------
The program outputs GOST R 34.11-2012 hash digest in hexadecimal format.
Each file listed on the command line is processed and hash is printed
for each one.  Stdin is read as input when executed without arguments.

    # ./gost3411-2012 -h
    Usage: gost3411-2012 [-25bhvqrte] [-s string] [files ...]

    Options:
       -2          Output 256-bit digest.
       -5          Output 512-bit digest (default).
       -t          Testing mode to produce hash of example
                   messages defined in standard.
       -b          Benchmark mode (to see how fast or slow
                   this implementation).
       -s string   Print a digest of the given string.
       -r          Reverses the format of the output.
                   This helps with visual diffs.
       -q          Quiet mode - only the digest is printed out.
       -e          Switch endianness when printing out
                   resulting hash.  Default: least significant
                   first.  With this options set all bytes in
                   resulting hash are printed in reverse
                   order, more precisely, most significant
                   first.


Using with Docker
-----------------
There is a pre-build Docker image of this software ready to use:

    $ docker run --rm adegtyarev/streebog gost3411-2012 -v
    gost3411-2012 0.12

Let's say you want to get a hash digest of LICENSE file in the current
directory.  Here is how:

    $ docker run --rm -v $PWD/LICENSE:/LICENSE:ro adegtyarev/streebog gost3411-2012 -2 /LICENSE
    GOST R 34.11-2012 (/LICENSE) = c73c0c79b345d0aa779efab878fbe8ff248ae666ac1fdd12b137e7f41ef2da82

You could also get that digest by using STDIN mode:

    $ cat LICENSE |docker run --rm -i adegtyarev/streebog gost3411-2012 -2
    c73c0c79b345d0aa779efab878fbe8ff248ae666ac1fdd12b137e7f41ef2da82


Compile-time options
--------------------
By default, a compiler defined in `CC` environment variable is used, falling
back to `cc`.  Compile the source with specified compiler:

    # make CC=clang

Special target `remake` may need to be used to overwrite recently compiled
up-to date binary:

    # make remake CC=icc

This will recompile sources from scratch using Intel C Compiler with
default flags.  If you need to adjust these compiler flags, try to set
them with `CFLAGS` knob:

    # make remake CC=icc CFLAGS="-O3"


API
---
The API to this implementation is quite straightforward and similar to
other hash function APIs.  Actually the CLI utility in this distribution
just use this API as underlying engine.  You may use this API to
implement GOST R 34.11-2012 in your application.

```c
GOST34112012Context
```

This is the hash context.  There should be one `GOST34112012Context`
for each object to be hashed.


```c
void GOST34112012Init(GOST34112012Context *CTX, const unsigned int digest_size);
```

Return initialized `GOST34112012Context` of specified hash size
(`digest_size`) on allocated memory block pointed by `CTX`.  Digest size
can be either 512 or 256.  Address of `CTX` must be 16-byte aligned.

```c
void GOST34112012Update(GOST34112012Context *CTX, const unsigned char *data, size_t len);
```

Hash some `data` of `len` bytes size.  The best performance results are
achieved when `len` is multiple of 64.
    
Note that this call does not modify original data in memory.  If
security is an issue, calling application should destroy that memory
block right after `GOST34112012Update()`, by e.g. `memset()` to zero.

```c
void GOST34112012Final(GOST34112012Context *CTX, unsigned char *digest);
```

Finalizes hashing process and set GOST R 34.11-2012 hash in memory block
pointed by `digest`.

```c
void GOST34112012Cleanup(GOST34112012Context *CTX);
```

The data in context including hash itself, buffer and internal
state zeroed-out.  Context totally destroyed and the object can't be
used anymore.  Calling application should `free()` memory used by this
context.

The following constants may be predefined somewhere in your application
code in order to adjust GOST R 34.11-2012 engine behavior:

* `__GOST3411_LITTLE_ENDIAN__`: define this constant on little-endian systems.

* `__GOST3411_BIG_ENDIAN__`: this constant will indicate big-endian system.

If neither of constants defined the engine defaults to little-endian
code.

* `__GOST3411_HAS_MMX__`: use MMX instructions to compute digest.

* `__GOST3411_HAS_SSE2__`: use SSE2 instruction set to speedup computation
of GOST R 34.11-2012 digest.

* `__GOST3411_HAS_SSE41__`: indicate to include SSE4.1 instructions set.

The best performance results achieved on SSE4.1 capable processors.  A slightly
less performance achieved on SSE2 capable processors.  The CLI utility in this
distribution tries its best to determine which of the instruction set to use.
It falls back to the portable code unless any of extensions detected.


Example of usage
----------------

```c
    #include <stdlib.h>
    #include <err.h>
    #include "gost3411-2012-core.h"

    ...

    GOST34112012Context *CTX;

    unsigned char digest[64];

    ...
        if (posix_memalign(&CTX, (size_t) 16, sizeof(GOST34112012Context)))
            err(EX_OSERR, NULL);

        GOST34112012Init(CTX, 512);
        ...
        GOST34112012Update(CTX, buffer, (size_t) bufsize);
        ...
        GOST34112012Update(CTX, buffer, (size_t) bufsize);
        ...
        /* call GOST34112012Update() for each block of data */
        ...
        GOST34112012Final(CTX, &digest[0]);
        ...
        /* You now have GOST R 34.11-2012 hash in 'digest' */
        ...
        GOST34112012Cleanup(CTX);
    ...

```

Portability notes
-----------------
...


Platforms tested
----------------
* FreeBSD x86/x86_64
* Linux   x86/x86_64
* Darwin  x86/x86_64
* Linux   powerpc


Performance
-----------
To measure performance of this implementation
[SUPERCOP](https://bench.cr.yp.to/supercop.html) toolkit has been used.  You
can set `SUPERCOP` environment variable to any value and then run `make
configure` to prepare this implementation to run on SUPERCOP.

    Intel(R) Pentium(R) CPU G6950 @ 2.80GHz    x86: 40 cycles per byte ( 70 MB/s)
    Intel(R) Pentium(R) CPU G6950 @ 2.80GHz x86_64: 36 cycles per byte ( 78 MB/s)
    Intel(R) Xeon(R)    CPU X5650 @ 2.67GHz x86_64: 31 cycles per byte ( 84 MB/s)
    Intel(R) Core(TM) i7-2600 CPU @ 3.40GHz x86_64: 28 cycles per byte (121 MB/s)


License
-------

This work is dual-licensed under BSD 2-Clause "Simplified" License and GNU
General Public License v2.0 (or later).  You can choose between one of them if
you use this software.

`SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0+`


Author
------
Alexey Degtyarev <alexey@renatasystems.org>
