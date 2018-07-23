#
# mruby/c  Makefile
#
# Copyright (C) 2015,2016 Kyushu Institute of Technology.
# Copyright (C) 2015,2016 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

SRC = array.rb numeric.rb range.rb
MRBC = mrbc

all: mrblib.c

mrblib.c: $(SRC)
	cat $(SRC) > mrblib.rb
	$(MRBC) -E -Bmrblib_bytecode mrblib.rb
