class MyBlock
  def initialize
    @result = Array.new
  end

  def func1
    yield
  end

  def each_double(array)
    array.each do |v|
      double(v)
    end
  end

  def double(val)
    @result << val * 2
  end

  def result
    @result
  end
end
