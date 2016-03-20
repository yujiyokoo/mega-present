def eachputs(ary)
  i = 0
  n = ary.size
  while i<n do
    puts ary[i]
    i = i + 1
  end
end

a = [0,1,2]
eachputs(a)

a.concat([3,4])
eachputs(a)

b = [2,3,4]
a.concat(b)
eachputs(a)
eachputs(b)
