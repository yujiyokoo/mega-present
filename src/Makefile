#
# mruby/c  src/Makefile
#
# Copyright (C) 2015-2018 Kyushu Institute of Technology.
# Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

CFLAGS = -Wall -Wpointer-arith -g -DMRBC_DEBUG  # -std=c99 -pedantic -pedantic-errors

COMMON_SRCS = alloc.c class.c console.c global.c keyvalue.c load.c rrt0.c static.c symbol.c value.c vm.c hal/hal.c
RUBY_LIB_SRCS = c_array.c c_hash.c c_numeric.c c_math.c c_range.c c_string.c mrblib.c

TARGET = libmrubyc.a
OBJS = $(COMMON_SRCS:.c=.o) $(RUBY_LIB_SRCS:.c=.o)


all:
	@if [ ! -e hal ]; then ln -s hal_posix hal; fi
	$(MAKE) $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?


vm.o: vm.c vm_config.h vm.h value.h alloc.h static.h global.h opcode.h \
  class.h symbol.h console.h hal/hal.h c_string.h c_range.h c_array.h \
  c_hash.h

hal.o: hal/hal.c hal/hal.h

value.o: value.c vm_config.h value.h class.h vm.h static.h global.h \
  symbol.h alloc.h c_string.h c_range.h c_array.h c_hash.h

alloc.o: alloc.c vm_config.h alloc.h vm.h value.h console.h hal/hal.h

keyvalue.o: keyvalue.c vm_config.h value.h alloc.h vm.h keyvalue.h

static.o: static.c vm_config.h static.h vm.h value.h global.h class.h \
  symbol.h

global.o: global.c vm_config.h value.h static.h vm.h global.h mrubyc.h \
  errorcode.h alloc.h symbol.h class.h c_array.h c_hash.h \
  c_numeric.h c_range.h c_string.h load.h console.h hal/hal.h \
  rrt0.h

class.o: class.c vm_config.h value.h vm.h class.h alloc.h static.h \
  global.h symbol.h console.h hal/hal.h c_array.h c_hash.h c_numeric.h \
  c_math.h c_string.h c_range.h

symbol.o: symbol.c vm_config.h value.h vm.h alloc.h static.h global.h \
  class.h symbol.h console.h hal/hal.h

load.o: load.c vm_config.h vm.h value.h load.h errorcode.h alloc.h

console.o: console.c vm_config.h console.h hal/hal.h

c_array.o: c_array.c vm_config.h value.h vm.h alloc.h static.h global.h \
  class.h c_array.h console.h hal/hal.h

c_numeric.o: c_numeric.c vm_config.h c_numeric.h vm.h value.h alloc.h \
  class.h static.h global.h console.h

c_math.o: c_math.c vm_config.h value.h

c_string.o: c_string.c vm_config.h value.h alloc.h vm.h static.h global.h \
  class.h c_string.h console.h hal/hal.h

c_range.o: c_range.c vm_config.h value.h alloc.h vm.h static.h global.h \
  class.h c_range.h console.h hal/hal.h

c_hash.o: c_hash.c vm_config.h value.h vm.h alloc.h static.h global.h \
  class.h c_array.h c_hash.h

rrt0.o: rrt0.c vm_config.h alloc.h vm.h value.h static.h global.h load.h \
  class.h console.h hal/hal.h rrt0.h


clean:
	@rm -Rf $(TARGET) $(OBJS) *~
