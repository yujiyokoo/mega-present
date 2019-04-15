class MySuper0
  attr_reader :a1, :a2

  def initialize( a1, a2 )
    @a1 = a1
    @a2 = a2
  end

  def method1( a1, a2 )
    @a1 = a1
    @a2 = a2
  end
end

class MySuper1 < MySuper0
  def initialize( a1 = 1, a2 = 2 )
    super
  end

  def method1( a1, a2 )
    a1 *= 2
    super( a1, a2*2 )
  end
end

class MySuper2 < MySuper1
  def method1( a1, a2 )
    super
    @a2 = 2222
  end
end
