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
  end
end
