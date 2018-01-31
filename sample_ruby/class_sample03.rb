class MyClass
  def initialize(n)
    puts "class init"
    puts n
  end
  def func
    puts "class func"
  end
end

a = MyClass.new(5)
a.func
