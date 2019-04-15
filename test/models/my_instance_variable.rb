class MyInstanceVariable
  attr_reader :r1, :r2
  attr_accessor :rw1, :rw2

  def method1(v1,v2)
    @r1 = v1
    @r2 = v2
  end
end
