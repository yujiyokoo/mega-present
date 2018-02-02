class MyClass
  def initialize
    puts "MyClass init"
  end
  def func
    puts "MyClass func"
  end
end

class MyClass2 < MyClass
  def initialize
    puts "MyClass2 init"
  end
end

class MyClass3 < MyClass2
end


a = MyClass.new
a.func

a = MyClass2.new
a.func

MyClass3.new.func
