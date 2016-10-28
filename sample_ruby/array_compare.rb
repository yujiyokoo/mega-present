# Array test for following methods:
#   ==

a = [1,2,3]
puts a==a

puts a==[1,2,3]

puts a==[1,2,3,4]

b = [1,2,3]
puts a==b

b[0] = 2
puts a==b

puts a!=b
