#
# mruby/c  src/Makefile
#
# Copyright (C) 2015,2016 Kyushu Institute of Technology.
# Copyright (C) 2015,2016 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

CFLAGS = -Wall -Wpointer-arith -g # -std=c99 -pedantic -pedantic-errors -DDEBUG

COMMON_SRCS = alloc.c class.c common.c console.c global.c load.c rrt0.c static.c symbol.c value.c vm.c hal/hal.c
RUBY_LIB_SRCS = c_array.c c_numeric.c c_range.c c_string.c
TARGET = libmrubyc.a
OBJS = $(COMMON_SRCS:.c=.o) $(RUBY_LIB_SRCS:.c=.o)


all:
	if [ ! -e hal ]; then ln -s hal_posix hal; fi
	$(MAKE) $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?

class.o: class.c value.h vm_config.h class.h vm.h static.h global.h \
  console.h c_array.h c_numeric.h c_string.h c_range.h
global.o: global.c value.h vm_config.h static.h vm.h global.h
load.o: load.c vm.h value.h vm_config.h load.h errorcode.h static.h \
  global.h common.h
vm.o: vm.c vm.h value.h vm_config.h alloc.h static.h global.h opcode.h \
  class.h symbol.h console.h common.h c_string.h c_range.h
static.o: static.c common.h static.h vm.h value.h vm_config.h global.h \
  class.h symbol.h
value.o: value.c value.h vm_config.h static.h vm.h global.h symbol.h \
  alloc.h
common.o: common.c common.h
symbol.o: symbol.c symbol.h value.h vm_config.h static.h vm.h global.h
console.o: console.c hal/hal.h console.h
alloc.o: alloc.c alloc.h vm.h value.h vm_config.h

rrt0.o: rrt0.c alloc.h vm.h value.h vm_config.h static.h global.h load.h \
  class.h rrt0.h hal/hal.h
hal.o: hal/hal.c hal/hal.h

c_array.o: c_array.c c_array.h vm.h value.h vm_config.h alloc.h class.h \
  static.h global.h
c_numeric.o: c_numeric.c vm_config.h c_numeric.h vm.h value.h alloc.h \
  class.h static.h global.h console.h
c_string.o: c_string.c c_string.h vm.h value.h vm_config.h alloc.h \
  class.h static.h global.h
c_range.o: c_range.c c_range.h vm.h value.h vm_config.h alloc.h class.h \
  static.h global.h


clean:
	@rm -Rf $(TARGET) $(OBJS) *~
