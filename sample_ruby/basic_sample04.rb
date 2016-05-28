# Create Array
a = Array.new(4,100)
puts "#{a.size} #{a[1]}"
a[0]=1
a[1]=2
a[2]=3
a[3]=4
a[4]=5
a[5]=6

#Show elements from two arrays
[1,2,3,4,5,6].reverse.zip(a).each {|i,j| puts "#{i} #{j}" }

