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

all: mrubyc mrubyc_sample mrubyc_concurrent

mrubyc: main.o 
	gcc $^ $(LDFLAGS) $(LDLIBS) -o $@

mrubyc_sample: main_sample.o 
	gcc $^ $(LDFLAGS) $(LDLIBS) -o $@

mrubyc_concurrent: main_concurrent.o 
	gcc $^ $(LDFLAGS) $(LDLIBS) -o $@

clean:
	@rm -f *.o mrubyc mrubyc_sample mrubyc_concurrent
