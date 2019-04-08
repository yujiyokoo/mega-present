# frozen_string_literal: true

class RangeTest < MrubycTestCase

  description "basic equality"
  def basic_case
    assert_equal (1..2), (1..2)
    assert_not_equal (1..2), (1...2)
    assert_not_equal (1..2), (1..3)
    assert_not_equal (1..3), (2..3)
  end

  description "=== operator"
  def equal3_case
    r = (1..3)
    assert_false r === 0
    assert_true  r === 1
    assert_true  r === 2
    assert_true  r === 3
    assert_false r === 4

    r = (1...3)
    assert_false r === 0
    assert_true  r === 1
    assert_true  r === 2
    assert_false r === 3
    assert_false r === 4
  end
end
