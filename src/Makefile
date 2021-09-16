#
# mruby/c  src/Makefile
#
# Copyright (C) 2015-2021 Kyushu Institute of Technology.
# Copyright (C) 2015-2021 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

include hal_selector.mk

TARGET = libmrubyc.a
CFLAGS += -Wall -Wpointer-arith -g  # -std=c99 -pedantic -pedantic-errors
SRCS = $(HAL_DIR)/hal.c alloc.c c_array.c c_hash.c c_math.c c_numeric.c \
	c_object.c c_range.c c_string.c class.c console.c error.c global.c \
	keyvalue.c load.c mrblib.c rrt0.c symbol.c value.c vm.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?

clean:
	@rm -f $(TARGET) $(OBJS) *~

clean_all:	clean
	@rm -f $(AUTOGEN_SYMBOL_TABLE) $(AUTOGEN_METHOD_TABLE)

autogen:
	@rm -f $(AUTOGEN_SYMBOL_TABLE) $(AUTOGEN_METHOD_TABLE)
	$(MAKE) $(AUTOGEN_SYMBOL_TABLE)


# Auto generated files.

MAKE_SYMBOL_TABLE ?= ../support/make_symbol_table.rb
MAKE_METHOD_TABLE ?= ../support/make_method_table.rb

AUTOGEN_SYMBOL_TABLE = symbol_builtin.h
AUTOGEN_METHOD_TABLE = method_table_array.h method_table_false.h \
  method_table_integer.h method_table_float.h method_table_hash.h \
  method_table_math.h method_table_nil.h method_table_object.h \
  method_table_proc.h method_table_range.h method_table_string.h \
  method_table_symbol.h method_table_true.h

$(AUTOGEN_SYMBOL_TABLE): $(AUTOGEN_METHOD_TABLE)
	$(MAKE_SYMBOL_TABLE) -a

method_table_array.h:	c_array.c
	$(MAKE_METHOD_TABLE) $?
method_table_false.h:	c_object.c
	$(MAKE_METHOD_TABLE) $?
method_table_integer.h:	c_numeric.c
	$(MAKE_METHOD_TABLE) $?
method_table_float.h:	c_numeric.c
	$(MAKE_METHOD_TABLE) $?
method_table_hash.h:	c_hash.c
	$(MAKE_METHOD_TABLE) $?
method_table_math.h:	c_math.c
	$(MAKE_METHOD_TABLE) $?
method_table_nil.h:	c_object.c
	$(MAKE_METHOD_TABLE) $?
method_table_object.h:	c_object.c
	$(MAKE_METHOD_TABLE) $?
method_table_proc.h:	c_object.c
	$(MAKE_METHOD_TABLE) $?
method_table_range.h:	c_range.c
	$(MAKE_METHOD_TABLE) $?
method_table_string.h:	c_string.c
	$(MAKE_METHOD_TABLE) $?
method_table_symbol.h:	symbol.c
	$(MAKE_METHOD_TABLE) $?
method_table_true.h:	c_object.c
	$(MAKE_METHOD_TABLE) $?


# File dependencies.

hal.o: $(HAL_DIR)/hal.c $(HAL_DIR)/hal.h
alloc.o: alloc.c vm_config.h vm.h value.h class.h keyvalue.h alloc.h \
  hal_selector.h $(HAL_DIR)/hal.h console.h
c_array.o: c_array.c vm_config.h value.h vm.h class.h keyvalue.h alloc.h \
  c_array.h c_string.h console.h method_table_array.h symbol_builtin.h
c_hash.o: c_hash.c vm_config.h value.h vm.h class.h keyvalue.h alloc.h \
  c_array.h c_hash.h c_string.h method_table_hash.h symbol_builtin.h
c_math.o: c_math.c vm_config.h value.h class.h keyvalue.h global.h \
  symbol.h symbol_builtin.h
c_numeric.o: c_numeric.c vm_config.h value.h class.h keyvalue.h console.h \
  c_numeric.h vm.h c_string.h method_table_integer.h symbol_builtin.h \
  method_table_float.h
c_object.o: c_object.c vm_config.h alloc.h value.h vm.h class.h \
  keyvalue.h symbol.h symbol_builtin.h c_string.h c_array.h c_hash.h \
  console.h opcode.h method_table_object.h method_table_proc.h \
  method_table_nil.h method_table_true.h method_table_false.h
c_range.o: c_range.c vm_config.h value.h alloc.h class.h keyvalue.h \
  c_range.h c_string.h console.h method_table_range.h symbol_builtin.h
c_string.o: c_string.c vm_config.h value.h vm.h class.h keyvalue.h \
  alloc.h symbol.h symbol_builtin.h c_array.h c_string.h console.h \
  method_table_string.h
class.o: class.c vm_config.h alloc.h value.h vm.h class.h keyvalue.h \
  symbol.h symbol_builtin.h global.h console.h load.h c_object.h \
  c_string.h c_array.h c_hash.h
console.o: console.c vm_config.h hal_selector.h $(HAL_DIR)/hal.h value.h \
  class.h keyvalue.h console.h symbol.h symbol_builtin.h c_string.h \
  c_array.h alloc.h c_hash.h c_range.h
error.o: error.c vm_config.h vm.h value.h class.h keyvalue.h error.h \
  c_string.h
global.o: global.c vm_config.h value.h global.h class.h keyvalue.h \
  symbol.h symbol_builtin.h console.h
keyvalue.o: keyvalue.c vm_config.h value.h alloc.h keyvalue.h
load.o: load.c vm_config.h vm.h value.h class.h keyvalue.h load.h alloc.h \
  symbol.h symbol_builtin.h c_string.h console.h
mrblib.o: mrblib.c
rrt0.o: rrt0.c vm_config.h alloc.h load.h value.h class.h keyvalue.h \
  global.h symbol.h symbol_builtin.h c_object.h vm.h console.h rrt0.h \
  hal_selector.h $(HAL_DIR)/hal.h
symbol.o: symbol.c vm_config.h symbol.h value.h symbol_builtin.h vm.h \
  class.h keyvalue.h alloc.h c_object.h c_string.h c_array.h console.h \
  method_table_symbol.h
value.o: value.c vm_config.h value.h class.h keyvalue.h c_string.h \
  c_range.h c_array.h alloc.h c_hash.h
vm.o: vm.c vm_config.h vm.h value.h class.h keyvalue.h alloc.h load.h \
  global.h opcode.h symbol.h symbol_builtin.h console.h c_object.h \
  c_string.h c_range.h c_array.h c_hash.h
