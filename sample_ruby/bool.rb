# coding: utf-8


puts "\n##### 三項演算子 #####"

v = true
puts "true " + (v ? "T" : "F")

v = false
puts "false " + (v ? "T" : "F")

v = nil
puts "nil " + (v ? "T" : "F")

v = 0
puts "0(整数) " + (v ? "T" : "F")

v = 1
puts "1(整数) " + (v ? "T" : "F")

v = 0.0
puts "0.0(小数) " + (v ? "T" : "F")

v = 0.0 / 0
puts "NaN " + (v ? "T" : "F")

v = "0"
puts "'0'(文字列) " + (v ? "T" : "F")

v = ""
puts "空文字列 " + (v ? "T" : "F")

v = []
puts "空配列 " + (v ? "T" : "F")

v = :sym
puts "symbol " + (v ? "T" : "F")


puts "\n##### !演算子 #####"

v = true
puts "true " + (!v ? "T" : "F")

v = false
puts "false " + (!v ? "T" : "F")

v = nil
puts "nil " + (!v ? "T" : "F")

v = 0
puts "0(整数) " + (!v ? "T" : "F")

v = 1
puts "1(整数) " + (!v ? "T" : "F")

v = 0.0
puts "0.0(小数) " + (!v ? "T" : "F")

v = 0.0 / 0
puts "NaN " + (!v ? "T" : "F")

v = "0"
puts "'0'(文字列) " + (!v ? "T" : "F")

v = ""
puts "空文字列 " + (!v ? "T" : "F")

v = []
puts "空配列 " + (!v ? "T" : "F")

v = :sym
puts "symbol " + (!v ? "T" : "F")


puts "\n##### !=演算子 #####"

v1,v2 = true,true
puts "true != true " + (!(v1 != v2) ? "T" : "F")

v1,v2 = true,false
puts "true != false " + (!(v1 != v2) ? "T" : "F")

v1,v2 = false,true
puts "false != true " + (!(v1 != v2) ? "T" : "F")

v1,v2 = false,false
puts "false != false " + (!(v1 != v2) ? "T" : "F")


v1,v2 = true,nil
puts "true != nil " + (!(v1 != v2) ? "T" : "F")

v1,v2 = nil,true
puts "nil != true " + (!(v1 != v2) ? "T" : "F")

v1,v2 = false,nil
puts "false != nil " + (!(v1 != v2) ? "T" : "F")

v1,v2 = nil,false
puts "nil != false " + (!(v1 != v2) ? "T" : "F")
