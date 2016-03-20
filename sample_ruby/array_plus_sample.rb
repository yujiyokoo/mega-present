def eachputs(ary)
  i = 0
  n = ary.size
  while i<n do
    puts ary[i]
    i = i + 1
  end
end


a = [0,1,2]
b = a + [3,4]

eachputs(a)
eachputs(b)

a = [10,20,30]
c = a + [40,50]
eachputs(b)
eachputs(c)
