def eachputs(ary)
  i = 0
  n = ary.size
  while i<n do
    puts ary[i]
    i = i + 1
  end
end

a = [1,2,3,4]
eachputs(a)

b = a.last(3)
eachputs(b)

c = a.last(1)
eachputs(c)
