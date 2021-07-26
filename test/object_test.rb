# frozen_string_literal: true

class ObjectTest < MrubycTestCase
  description 'alias'
  def alias_case
    assert_equal "foo", _orig_foo()
    assert_equal "foofoo", foo()
  end

  description 'Object class'
  def all
    # nil?
    assert_equal true,  nil.nil?
    assert_equal false, true.nil?
    assert_equal false, false.nil?
    assert_equal nil, puts("Hello World!")
  end

  description 'Object#p method without arg'
  def p_returns_args_0
    a = p
    assert_equal nil, a
  end

  description 'Object#p method with an arg'
  def p_returns_args_1
    a = p 1
    assert_equal 1, a
  end

  description 'Object#p method with multiple args'
  def p_returns_args_m
    a = p 1, "hello", :hey
    assert_equal [1, "hello", :hey], a
  end
end
