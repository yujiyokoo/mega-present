def eachputs(ary)
	i = 0
	n = ary.size
	while i<n do
		puts ary[i]
		i = i + 1
	end
end

a = [1, 2, 3]
eachputs a

a.replace([4,5])
eachputs a

b=[6,7]
a.replace(b)
eachputs a
