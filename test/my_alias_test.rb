# frozen_string_literal: true

class MyAliasTest < MrubycTestCase
  def setup
    @obj = MyAlias.new
  end

  description 'alias'
  def method1_case
    assert_equal "MyClass#method1", @obj.method1
    assert_equal "MyClass#method1", @obj.method1_alias
  end

  description 'override original method'
  def method2_case
    assert_equal "MyClass#method2_alternate", @obj.method2
    assert_equal "MyClass#method2", @obj.method2_alias
  end

  description 'override alias method'
  def method3_case
    assert_equal "MyClass#method3_alternate", @obj.method3
    assert_equal "MyClass#method3_alternate", @obj.method3_alias
  end
end
