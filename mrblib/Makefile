#
# mruby/c  Makefile
#
# Copyright (C) 2015-2021 Kyushu Institute of Technology.
# Copyright (C) 2015-2021 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

SRC = array.rb global.rb hash.rb numeric.rb object.rb range.rb string.rb
OUTPUT = ../src/mrblib.c
MRBC ?= mrbc


all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(MRBC) -E -Bmrblib_bytecode --remove-lv -o$(OUTPUT) $(SRC)

clean:
	@rm -f *~

distclean: clean
	@rm -f $(OUTPUT)
