#!/usr/bin/env ruby
#
# symbol name to enum symbol converter.
#
#  Copyright (C) 2015-2020 Kyushu Institute of Technology.
#  Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#
#

RENAME_CHAR = {
  "!"=>"NOT",   # or "EXC"
  "%"=>"MOD",
  "&"=>"AND",
  "*"=>"MUL",
  "+"=>"PLUS",
  "-"=>"MINUS",
  "/"=>"DIV",
  "<"=>"LT",
  "="=>"EQ",
  ">"=>"GT",
  "?"=>"Q",
  "@"=>"AT",
  "["=>"BLL",
  "]"=>"BLR",
  "^"=>"HAT",
  "|"=>"OR",
  "~"=>"TILDE"
}

##
# rename for symbol
#
def rename_for_symbol(s)
  return "#{$1}_EXC"  if /^(.+)!$/ =~ s

  r = ""
  s.each_char {|ch|
    if !RENAME_CHAR[ch]
      r << ch
    else
      r << "_"  if !r.empty?
      r << RENAME_CHAR[ch]
    end
  }
  return r
end
