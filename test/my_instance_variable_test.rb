# frozen_string_literal: true

class MyInstanceVariableTest < MrubycTestCase

  description 'instance variable'
  def all
    # 初期値は、nilであること
    obj1 = MyInstanceVariable.new
    assert_equal [nil,nil,nil,nil], [obj1.r1, obj1.r2, obj1.rw1, obj1.rw2]

    # メソッドを通して、インスタンス変数に値が設定できること
    obj1.method1( 111, 222 )
    assert_equal [111,222,nil,nil], [obj1.r1, obj1.r2, obj1.rw1, obj1.rw2]

    # attr_accessorで書き込みができること
    obj1.rw1 = 333
    obj1.rw2 = 444
    assert_equal [111,222,333,444], [obj1.r1, obj1.r2, obj1.rw1, obj1.rw2]

    # インスタンス間で干渉がないこと
    obj2 = MyInstanceVariable.new
    assert_equal [111,222,333,444], [obj1.r1, obj1.r2, obj1.rw1, obj1.rw2]
    assert_equal [nil,nil,nil,nil], [obj2.r1, obj2.r2, obj2.rw1, obj2.rw2]

    obj2.rw1 = 2211
    obj2.rw2 = 2222
    assert_equal [111,222,333,444], [obj1.r1, obj1.r2, obj1.rw1, obj1.rw2]
    assert_equal [nil,nil,2211,2222], [obj2.r1, obj2.r2, obj2.rw1, obj2.rw2]
  end
end
