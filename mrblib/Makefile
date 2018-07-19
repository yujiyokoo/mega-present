#
# mruby/c  Makefile
#
# Copyright (C) 2015,2016 Kyushu Institute of Technology.
# Copyright (C) 2015,2016 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

SRC = numeric.rb
MRBC = mrbc

all: mrblib.c

mrblib.c: $(SRC)
	cat $(SRC) > mrblib.rb
	$(MRBC) -v -E -Bmrblib_bytecode mrblib.rb > mrblib.txt
