FROM amd64/alpine:3.19.9
RUN echo "x86" > /etc/apk/arch
RUN apk add --no-cache wine freetype xvfb gnutls

ENV WINEARCH=win32
ENV WINEDEBUG=fixme-all

RUN adduser -u 1000 -D app
USER app

RUN winecfg
WORKDIR /app

# build a 32bit runnable plutonium-updater as theres no release for it and/or the latest release is doesn't work on alpine linux 64bit
USER root
RUN apk add --no-cache cargo cmake git
USER app
RUN git clone https://github.com/mxve/plutonium-updater.rs
WORKDIR /app/plutonium-updater.rs
RUN git checkout 209738b45144427f0e9e7248717ba30784cbfd5b
RUN cargo build --release
RUN cp target/release/plutonium-updater ../plutonium-updater
WORKDIR /app
RUN rm -rf plutonium-updater.rs
USER root
RUN apk del cargo cmake git
USER app

COPY scripts/docker/entrypoint.sh entrypoint.sh
VOLUME [ "/app/plutonium" ]
RUN mkdir -p plutonium/plugins
COPY build/bin/Release/plutonium_plugin_base.dll plutonium/plugins/plutonium_plugin_base.dll

EXPOSE 28960/udp
ENTRYPOINT [ "./entrypoint.sh" ]
