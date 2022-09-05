FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        wget \
        texinfo \
        default-jre && \
    apt-get clean

WORKDIR /tmp
RUN wget https://github.com/kubilus1/gendev/releases/download/0.7.1/gendev_0.7.1_all.deb
RUN dpkg -i ./gendev_0.7.1_all.deb
COPY gendev-mega-mrubyc.patch /tmp

WORKDIR /opt/gendev
RUN patch -p2 < /tmp/gendev-mega-mrubyc.patch

