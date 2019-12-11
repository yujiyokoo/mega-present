# frozen_string_literal: true

class MyBlockTest < MrubycTestCase

  def setup
    @obj = MyBlock.new
  end

  description 'basic_yield'
  def yeald_case
    $count = 0
    @obj.func1 { $count += 1 }
    assert_equal 1, $count
  end

  description "instance method inside block"
  def instance_method_case
    @obj.each_double([1, 2, 3])
    assert_equal [2, 4, 6], @obj.result
  end
end
