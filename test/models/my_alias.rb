class MyAlias
  # simple aliase
  def method1
    return "MyClass#method1"
  end
  alias :method1_alias :method1

  # overriding original method
  def method2
    return "MyClass#method2"
  end
  alias :method2_alias :method2
  def method2
    return "MyClass#method2_alternate"
  end

  # overriding alias method
  def method3
    return "MyClass#method3"
  end
  alias :method3_alias :method3
  def method3
    return "MyClass#method3"
  end
  alias :method3_alias :method3
  def method3
    return "MyClass#method3_alternate"
  end
  alias :method3_alias :method3
end
