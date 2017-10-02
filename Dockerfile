FROM    telephoneorg/debian:stretch


MAINTAINER Joe Black <me@joeblack.nyc>

RUN     apt-get update -qq && \
            apt-get install build-essential -qqy && \
            apt-clean --aggressive

RUN     mkdir -p /build/overridehostname
COPY    entrypoint /

WORKDIR /build/overridehostname

VOLUME  ["/build/overridehostname"]

CMD     ["/entrypoint"]
