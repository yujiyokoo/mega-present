from ruby:2.7.2-slim

RUN apt update && apt -y upgrade
RUN apt install -y bison make git gcc gcc-arm-linux-gnueabi qemu qemu-kvm qemu-system-arm

RUN gem update --system

RUN git clone https://github.com/mruby/mruby /root/mruby
ARG MRUBY_TAG
RUN cd /root/mruby; git checkout $MRUBY_TAG; make

VOLUME /root/mrubyc
COPY Gemfile /root/mrubyc/
COPY Gemfile.lock /root/mrubyc/
WORKDIR /root/mrubyc
RUN bundle install
ENV CFLAGS="-DMRBC_USE_MATH=1 -DMAX_SYMBOLS_COUNT=500"
CMD ["bundle", "exec", "mrubyc-test", "-e", "100", "-p", "/root/mruby/build/host/bin/mrbc"]
