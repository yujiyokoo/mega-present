#
# mruby/c  sample_c/Makefile
#
# Copyright (C) 2015,2016 Kyushu Institute of Technology.
# Copyright (C) 2015,2016 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

CPPFLAGS = -g -I ../src -Wall -Wpointer-arith
LDFLAGS = -L ../src
LDLIBS = -lmrubyc

LIBMRUBYC = ../src/libmrubyc.a

all: mrubyc mrubyc_sample mrubyc_concurrent

mrubyc: main.o $(LIBMRUBYC)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

mrubyc_sample: main_sample.o $(LIBMRUBYC)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

mrubyc_concurrent: main_concurrent.o $(LIBMRUBYC)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

clean:
	@rm -f *.o mrubyc mrubyc_sample mrubyc_concurrent
