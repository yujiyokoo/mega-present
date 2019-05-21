# frozen_string_literal: true

class BoolTest < MrubycTestCase

  description 'true / false'
  def basic_case
    v = true
    assert_true( v )

    v = false
    assert_false( v )

    v = nil
    #commented out by hasumi#assert_false( v )
    assert_nil( v )

    v = 0
    #commented out by hasumi#assert_true( v )
    assert( v )

    v = 1
    #commented out by hasumi#assert_true( v )
    assert( v )

    v = 0.0
    #commented out by hasumi#assert_true( v )
    assert( v )

    v = 0.0 / 0
    #commented out by hasumi#assert_true( v )
    assert( v )

    v = "0"
    #commented out by hasumi#assert_true( v )
    assert( v )

    v = ""
    #commented out by hasumi#assert_true( v )
    assert( v )

    v = []
    #commented out by hasumi#assert_true( v )
    assert( v )

    v = :sym
    #commented out by hasumi#assert_true( v )
    assert( v )
  end

  description "! (negation)"
  def negation_case
    v = true
    assert_false( !v )

    v = false
    assert_true( !v )

    v = nil
    assert_true( !v )

    v = 0
    assert_false( !v )

    v = 1
    assert_false( !v )

    v = 0.0
    assert_false( !v )

    v = 0.0 / 0
    assert_false( !v )

    v = "0"
    assert_false( !v )

    v = ""
    assert_false( !v )

    v = []
    assert_false( !v )

    v = :sym
    assert_false( !v )
  end


  description "!= negation operator"
  def negation_operator_case
    v1,v2 = true,true
    assert_equal( v1, v2 )

    v1,v2 = true,false
    assert_not_equal( v1, v2 )

    v1,v2 = false,true
    assert_not_equal( v1, v2 )

    v1,v2 = false,false
    assert_equal( v1, v2 )

    v1,v2 = true,nil
    assert_not_equal( v1, v2 )

    v1,v2 = nil,true
    assert_not_equal( v1, v2 )

    v1,v2 = false,nil
    assert_not_equal( v1, v2 )

    v1,v2 = nil,false
    assert_not_equal( v1, v2 )
  end

end
