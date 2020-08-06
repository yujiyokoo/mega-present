#!/usr/bin/env ruby
#
# create built-in symbol table in ROM
#
#  Copyright (C) 2015-2020 Kyushu Institute of Technology.
#  Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#
# (usage)
# ruby make_symbol_table.rb > src/symbol_builtin.h
#  or
# ruby make_symbol_table.rb SYMBOL_LIST.TXT > src/symbol_builtin.h
#

all_symbols = ["Object", "new", "!", "!=", "<=>", "===", "class", "dup", "block_given?", "is_a?", "kind_of?", "nil?", "p", "print", "puts", "raise", "object_id", "instance_methods", "instance_variables", "memory_statistics", "attr_reader", "attr_accessor", "sprintf", "printf", "inspect", "to_s", "Proc", "call", "NilClass", "to_i", "to_a", "to_h", "to_f", "TrueClass", "FalseClass", "Symbol", "all_symbols", "id2name", "to_sym", "Fixnum", "[]", "+@", "-@", "**", "%", "&", "|", "^", "~", "<<", ">>", "abs", "chr", "Float", "String", "+", "*", "size", "length", "[]=", "b", "clear", "chomp", "chomp!", "empty?", "getbyte", "index", "ord", "slice!", "split", "lstrip", "lstrip!", "rstrip", "rstrip!", "strip", "strip!", "intern", "tr", "tr!", "start_with?", "end_with?", "include?", "Array", "at", "delete_at", "count", "first", "last", "push", "pop", "shift", "unshift", "min", "max", "minmax", "join", "Range", "exclude_end?", "Hash", "delete", "has_key?", "has_value?", "key", "keys", "merge", "merge!", "values", "Exception", "message", "StandardError", "RuntimeError", "ZeroDivisionError", "ArgumentError", "IndexError", "TypeError", "collect", "map", "collect!", "map!", "delete_if", "each", "each_index", "each_with_index", "reject!", "reject", "sort!", "sort", "RUBY_VERSION", "MRUBYC_VERSION", "times", "loop", "each_byte", "each_char"]


##
# main
#
if ARGV[0]
  # read specified symbol list.
  all_symbols.clear

  File.open( ARGV[0] ) {|file|
    while s = file.gets
      s.chomp!
      all_symbols << s
    end
  }
end

all_symbols << "initialize"
all_symbols.sort!
all_symbols.uniq!

if all_symbols.size > 256
  STDERR.puts "Symbol table size must less than 256"
  exit
end

puts "/*"
puts " This file was generated automatically by make_symbol_table.rb"
puts "*/"
puts
puts "static const char *builtin_symbols[] = {"
all_symbols.each {|s|
  printf %!  "%s",\n!, s
}
puts "};"
