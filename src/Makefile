#
# mruby/c  src/Makefile
#
# Copyright (C) 2015,2016 Kyushu Institute of Technology.
# Copyright (C) 2015,2016 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

#CC = clang
CFLAGS = -O0 -Wall -g # -std=c99 -pedantic -pedantic-errors -DDEBUG

COMMON_OBJ = class.o global.o load.o vm.o static.o value.o common.o symbol.o
RUBY_LIBS = c_array.o c_numeric.o

all: libmrubyc.a

clean:
	@rm -Rf *.o *.a html

libmrubyc.a: $(COMMON_OBJ) $(RUBY_LIBS)
	$(AR) r $@ $(COMMON_OBJ) $(RUBY_LIBS)

class.o:	class.h value.h static.h
common.o: common.h
global.o: global.h
load.o:		load.h vm.h vm_config.h errorcode.h static.h value.h
main.o:		vm.h load.h errorcode.h static.h
static.o:	static.h vm_config.h common.h
symbol.o: symbol.h
value.o:	value.h static.h vm_config.h
vm.o:		vm.h static.h vm_config.h opcode.h class.h

c_array.o: c_array.h
c_numeric.o: c_numeric.h

main_sample.o:		vm.h load.h errorcode.h static.h
main_concurrent.o: vm.h load.h errorcode.h static.h
