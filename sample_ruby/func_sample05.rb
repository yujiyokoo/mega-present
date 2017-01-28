#  call with block

def func
  yield
end

func { puts "hello" }

a = 3
func { a = 5 }
puts a

