def eachputs(ary)
	i = 0
	n = ary.size
	while i<n do
		puts ary[i]
		i = i + 1
	end
end

a = [1,2,3]
eachputs a

puts a.pop
eachputs a
