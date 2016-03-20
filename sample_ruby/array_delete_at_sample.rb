def eachputs(ary)
  i = 0
  n = ary.size
  while i<n do
    puts ary[i]
    i = i + 1
  end
end

a = [1,2,3]

b = a.delete_at(0)
eachputs(a)
eachputs(b)

#a.delete_at(5)
#eachputs(a)
