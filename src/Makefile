#
# mruby/c  src/Makefile
#
# Copyright (C) 2015-2020 Kyushu Institute of Technology.
# Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

ifdef MRBC_USE_POSIX
  HAL_DIR = hal_posix
endif
ifdef MRBC_USE_HAL_PSOC5LP
  HAL_DIR = hal_psoc5lp
endif
ifdef MRBC_USE_HAL_ESP32
  HAL_DIR = hal_esp32
endif
ifdef MRBC_USE_HAL_PIC24
  HAL_DIR = hal_pic24
endif
ifdef MRBC_USE_HAL_USER_RESERVED
  HAL_DIR = hal_user_reserved
endif
ifndef HAL_DIR
  HAL_DIR = hal_posix
endif

MAKE_SYMBOL_TABLE = ../support/make_symbol_table.rb
MAKE_METHOD_TABLE = ../support/make_method_table.rb

TARGET = libmrubyc.a
CFLAGS += -Wall -Wpointer-arith -g  # -std=c99 -pedantic -pedantic-errors
SRCS = $(HAL_DIR)/hal.c alloc.c keyvalue.c value.c global.c class.c symbol.c \
  error.c  console.c c_array.c c_hash.c c_math.c c_numeric.c c_object.c \
  c_range.c c_string.c mrblib.c vm.c load.c rrt0.c
OBJS = $(SRCS:.c=.o)


all:
	$(MAKE) $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?

clean:
	@rm -Rf $(TARGET) $(OBJS) *~

symboltable:
	$(MAKE_SYMBOL_TABLE) -a -v

methodtable:
	$(MAKE_METHOD_TABLE) c_array.c
	$(MAKE_METHOD_TABLE) c_hash.c
	$(MAKE_METHOD_TABLE) c_math.c
	$(MAKE_METHOD_TABLE) c_numeric.c
	$(MAKE_METHOD_TABLE) c_object.c
	$(MAKE_METHOD_TABLE) c_range.c
	$(MAKE_METHOD_TABLE) c_string.c
	$(MAKE_METHOD_TABLE) symbol.c


hal.o: $(HAL_DIR)/hal.c $(HAL_DIR)/hal.h
alloc.o: alloc.c vm_config.h vm.h value.h class.h keyvalue.h alloc.h \
  hal_selector.h $(HAL_DIR)/hal.h console.h
c_array.o: c_array.c vm_config.h value.h vm.h class.h keyvalue.h alloc.h \
  c_array.h c_string.h console.h hal_selector.h $(HAL_DIR)/hal.h opcode.h \
  method_table_array.h symbol_builtin.h
c_hash.o: c_hash.c vm_config.h value.h vm.h class.h keyvalue.h alloc.h \
  c_array.h c_hash.h c_string.h method_table_hash.h symbol_builtin.h
c_math.o: c_math.c vm_config.h value.h class.h keyvalue.h
c_numeric.o: c_numeric.c vm_config.h opcode.h value.h class.h keyvalue.h \
  console.h hal_selector.h $(HAL_DIR)/hal.h c_numeric.h vm.h c_string.h \
  method_table_fixnum.h symbol_builtin.h method_table_float.h
c_object.o: c_object.c vm_config.h alloc.h value.h vm.h class.h \
  keyvalue.h symbol.h load.h c_object.h c_string.h c_array.h c_hash.h \
  console.h hal_selector.h $(HAL_DIR)/hal.h opcode.h \
  method_table_object.h symbol_builtin.h method_table_proc.h \
  method_table_nil.h method_table_true.h method_table_false.h
c_range.o: c_range.c vm_config.h value.h alloc.h class.h keyvalue.h \
  c_range.h c_string.h console.h hal_selector.h $(HAL_DIR)/hal.h opcode.h \
  method_table_range.h symbol_builtin.h
c_string.o: c_string.c vm_config.h value.h vm.h class.h keyvalue.h \
  alloc.h symbol.h c_array.h c_string.h console.h hal_selector.h \
  $(HAL_DIR)/hal.h method_table_string.h symbol_builtin.h
class.o: class.c vm_config.h alloc.h value.h vm.h class.h keyvalue.h \
  symbol.h global.h console.h hal_selector.h $(HAL_DIR)/hal.h
console.o: console.c vm_config.h value.h class.h keyvalue.h console.h \
  hal_selector.h $(HAL_DIR)/hal.h symbol.h c_string.h c_array.h alloc.h \
  c_hash.h c_range.h
error.o: error.c vm_config.h vm.h value.h class.h keyvalue.h error.h \
  c_string.h
global.o: global.c vm_config.h value.h global.h class.h keyvalue.h \
  symbol.h console.h hal_selector.h $(HAL_DIR)/hal.h
keyvalue.o: keyvalue.c vm_config.h value.h alloc.h keyvalue.h
load.o: load.c vm_config.h vm.h value.h class.h keyvalue.h load.h alloc.h \
  console.h hal_selector.h $(HAL_DIR)/hal.h
mrblib.o: mrblib.c
rrt0.o: rrt0.c vm_config.h alloc.h load.h class.h value.h keyvalue.h \
  global.h symbol.h c_object.h vm.h console.h hal_selector.h \
  $(HAL_DIR)/hal.h rrt0.h
symbol.o: symbol.c vm_config.h value.h vm.h class.h keyvalue.h alloc.h \
  symbol.h c_object.h c_string.h c_array.h console.h hal_selector.h \
  $(HAL_DIR)/hal.h symbol_builtin.h
value.o: value.c vm_config.h value.h class.h keyvalue.h c_string.h \
  c_range.h c_array.h alloc.h c_hash.h
vm.o: vm.c vm_config.h vm.h value.h class.h keyvalue.h alloc.h load.h \
  global.h opcode.h symbol.h console.h hal_selector.h $(HAL_DIR)/hal.h \
  c_object.h c_string.h c_range.h c_array.h c_hash.h
