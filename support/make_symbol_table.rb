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
# ruby make_symbol_table.rb [option]
#
#  -o output filename.
#  -i input symbol list filename.
#  -a Targets all .c files in the current directory.
#  -v verbose
#

require "optparse"
require_relative "common_sub"

OUTPUT_FILENAME = "symbol_builtin.h"
APPEND_SYMBOL = ["initialize", "Exception", "message", "StandardError", "RuntimeError", "ZeroDivisionError", "ArgumentError", "IndexError", "TypeError", "collect", "map", "collect!", "map!", "delete_if", "each", "each_index", "each_with_index", "reject!", "reject", "sort!", "sort", "RUBY_VERSION", "MRUBYC_VERSION", "times", "loop", "each_byte", "each_char"]


##
# verbose print
#
def vp( s, level = 1 )
  puts s  if $options[:v] >= level
end


##
# parse command line option
#
def get_options
  opt = OptionParser.new
  ret = {:i=>[], :v=>0}

  opt.on("-i input file") {|v| ret[:i] << v }
  opt.on("-o output file", "(default #{OUTPUT_FILENAME})") {|v| ret[:o] = v }
  opt.on("-a", "targets all .c files") {|v| ret[:a] = v }
  opt.on("-v", "verbose mode") {|v| ret[:v] += 1 }
  opt.parse!(ARGV)
  return ret

rescue OptionParser::MissingArgument =>ex
  STDERR.puts ex.message
  return nil
end


##
# read *.c file and extract symbols.
#
def fetch_builtin_symbol( filename )
  ret = []
  vp("Process '#{filename}'")

  File.open( filename ) {|file|
    while src = get_method_table_source( file )
      param = parse_source_string( src )
      exit 1 if !param
      exit 1 if !check_error( param )

      vp("Found class #{param[:class]}, #{param[:methods].size} methods.")
      ret << param[:class]
      param[:methods].each {|m| ret << m[:name] }
    end
  }

  return ret
end



##
# main
#
$options = get_options()
exit if !$options

# read source file(s)
if !$options[:i].empty?
  source_files = $options[:i]
elsif $options[:a]
  source_files = Dir.glob("*.c")
else
  STDERR.puts "File not given."
  exit 1
end

all_symbols = []
source_files.each {|filename|
  all_symbols.concat( fetch_builtin_symbol( filename ) )
}
all_symbols.concat( APPEND_SYMBOL )
all_symbols.sort!
all_symbols.uniq!
vp("Total number of built-in symbols: #{all_symbols.size}")

if all_symbols.size > 256
  STDERR.puts "Symbol table size must be less than 256"
  exit 1
end

# output symbol table file.
output_filename = $options[:o] || OUTPUT_FILENAME
vp("Output file '#{output_filename}'")
begin
  file = File.open( output_filename, "w" )
rescue Errno::ENOENT
  puts "File can't open. #{output_filename}"
  exit 1
end

file.puts "/* Auto generated by make_symbol_table.rb */"
file.puts "#ifndef MRBC_SRC_SYMBOL_BUILTIN_H_"
file.puts "#define MRBC_SRC_SYMBOL_BUILTIN_H_"
file.puts

file.puts "#if defined(MRBC_DEFINE_SYMBOL_TABLE)"
file.puts "static const char *builtin_symbols[] = {"
all_symbols.each {|s|
  file.puts %!  "#{s}",!
}
file.puts "};"
file.puts "#endif"
file.puts

file.puts "enum {"
all_symbols.each_with_index {|s,i|
  file.puts "  MRBC_SYMID_#{rename_for_symbol(s)} = #{i},"
}
file.puts "};"
file.puts "#endif"

file.close
vp("Done")