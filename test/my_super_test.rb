# frozen_string_literal: true

class MySuperTest < MrubycTestCase

  description "MySuper1"
  def super_1_case
    obj = MySuper1.new()
    assert_equal 1, obj.a1
    assert_equal 2, obj.a2

    obj.method1( 11, 22 )
    assert_equal 22, obj.a1
    assert_equal 44, obj.a2
  end

  description "MySuper2"
  def super_2_case
    pend
    obj = MySuper2.new()
    obj.method1( 11, 22 )
    assert_equal 22, obj.a1
    assert_equal 2222, obj.a2
  end

end
