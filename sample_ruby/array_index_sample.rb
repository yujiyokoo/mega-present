# Array test for following methods:
#   index
#   []
#   []=

a = [1,3,5,7]
puts a.index(1)
puts a.index(3)
puts a.index(7)
puts a.index(2)
puts a.index(6)

puts a[0]
puts a[3]
puts a[7]

puts a.at(0)
puts a.at(3)
puts a.at(7)

a[0] = 9
puts a[0]
