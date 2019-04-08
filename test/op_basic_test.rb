# frozen_string_literal: true

class OpBasicTest < MrubycTestCase

  description "op_add"
  def op_add_case
    a = 1
    b = 2
    assert_equal( a + b, 3 )

    a = 1
    b = 2.0
    assert_equal( a + b, 3.0 )

    a = 1.0
    b = 2
    assert_equal( a + b, 3.0 )

    a = 1.0
    b = 2.0
    assert_equal( a + b, 3.0 )
  end

  description "op_addi"
  def op_addi_case
    a = 1
    assert_equal( a + 2, 3 )
    assert_equal( 1 + 2, 3 )

    a = 1.0
    assert_equal( a + 2, 3.0 )
    assert_equal( 1.0 + 2, 3.0 )
  end

  description "op_sub"
  def op_sub_case
    a = 1
    b = 2
    assert_equal( a - b, -1 )

    a = 1
    b = 2.0
    assert_equal( a - b, -1.0 )

    a = 1.0
    b = 2
    assert_equal( a - b, -1.0 )

    a = 1.0
    b = 2.0
    assert_equal( a - b, -1.0 )
  end

  description "op_subi"
  def op_subi_case
    a = 1
    assert_equal( a - 2, -1 )

    a = 1.0
    assert_equal( a - 2, -1.0 )
  end

  description "op_mul"
  def op_mul_case
    a = 2
    b = 3
    assert_equal a * b, 6

    a = 2
    b = 3.0
    assert_equal a * b, 6.0

    a = 2.0
    b = 3
    assert_equal a * b, 6.0

    a = 2.0
    b = 3.0
    assert_equal a * b, 6.0
  end

  description "op_lt"
  def op_lt_case
    a = 1
    b = 2
    assert_true( a < b )
    assert_false( b < a )

    a = 1
    b = 2.0
    assert_true( a < b )
    assert_false( b < a )

    a = 1.0
    b = 2
    assert_true( a < b )
    assert_false( b < a )

    a = 1.0
    b = 2.0
    assert_true( a < b )
    assert_false( b < a )

    a = 1
    b = 1
    assert_false( a < b )

    a = 1
    b = 1.0
    assert_false( a < b )

    a = 1.0
    b = 1
    assert_false( a < b )

    a = 1.0
    b = 1.0
    assert_false( a < b )
  end

  description "op_le"
  def op_le_case
    a = 1
    b = 2
    assert_true( a <= b )
    assert_false( b <= a )

    a = 1
    b = 2.0
    assert_true( a <= b )
    assert_false( b <= a )

    a = 1.0
    b = 2
    assert_true( a <= b )
    assert_false( b <= a )

    a = 1.0
    b = 2.0
    assert_true( a <= b )
    assert_false( b <= a )

    a = 1
    b = 1
    assert_true( a <= b )

    a = 1
    b = 1.0
    assert_true( a <= b )

    a = 1.0
    b = 1
    assert_true( a <= b )

    a = 1.0
    b = 1.0
    assert_true( a <= b )
  end

  description "op_gt"
  def op_gt_case
    a = 1
    b = 2
    assert_false( a > b )
    assert_true( b > a )

    a = 1
    b = 2.0
    assert_false( a > b )
    assert_true( b > a )

    a = 1.0
    b = 2
    assert_false( a > b )
    assert_true( b > a )

    a = 1.0
    b = 2.0
    assert_false( a > b )
    assert_true( b > a )

    a = 1
    b = 1
    assert_false( a > b )

    a = 1
    b = 1.0
    assert_false( a > b )

    a = 1.0
    b = 1
    assert_false( a > b )

    a = 1.0
    b = 1.0
    assert_false( a > b )
  end

  description "op_ge"
  def op_ge_case
    a = 1
    b = 2
    assert_false( a >= b )
    assert_true( b >= a )

    a = 1
    b = 2.0
    assert_false( a >= b )
    assert_true( b >= a )

    a = 1.0
    b = 2
    assert_false( a >= b )
    assert_true( b >= a )

    a = 1.0
    b = 2.0
    assert_false( a >= b )
    assert_true( b >= a )

    a = 1
    b = 1
    assert_true( a >= b )

    a = 1
    b = 1.0
    assert_true( a >= b )

    a = 1.0
    b = 1
    assert_true( a >= b )

    a = 1.0
    b = 1.0
    assert_true( a >= b )
  end

end
