#
# mruby/c  Makefile
#
# Copyright (C) 2015-2021 Kyushu Institute of Technology.
# Copyright (C) 2015-2021 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

TARGET = ../src/mrblib.c
SRC = array.rb global.rb hash.rb numeric.rb object.rb range.rb string.rb
MRBC ?= mrbc


all: $(TARGET)

$(TARGET): $(SRC)
	$(MRBC) -Bmrblib_bytecode --remove-lv -o$(TARGET) $(SRC)

clean:
	@rm -f *~

distclean:
	@rm -f $(TARGET) *~
