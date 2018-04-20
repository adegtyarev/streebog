FROM    alpine:latest

COPY    Makefile \
        configure \
        gost3411-2012*.* \
        /src/streebog/

COPY    auto /src/streebog/auto

WORKDIR /src/streebog

RUN     apk add --no-cache --virtual .build-deps \
            build-base && \
        make test && \
        install -v -s -m 755 gost3411-2012 /usr/bin/gost3411-2012 && \
        apk del .build-deps

USER    daemon

CMD     gost3411-2012
