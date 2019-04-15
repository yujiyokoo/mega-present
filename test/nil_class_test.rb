# frozen_string_literal: true

class NilClassTest < MrubycTestCase

  description 'nil?'
  def is_nil_case
    assert_equal( true,  nil.nil? )
  end

  description "variety of nil.to_SOMETHING"
  def nil_to_something_case
    assert_equal( 0,   nil.to_i )
    assert_equal( "",  nil.to_s )
    assert_equal( 0.0, nil.to_f )
    assert_equal( [],  nil.to_a )
    assert_equal( {},  nil.to_h )
  end

    #assert_equal( false, nil & true )
    #assert_equal( false, nil & false )
    #assert_equal( true,  nil ^ true )
    #assert_equal( false, nil ^ false )
    #assert_equal( true,  nil | true )
    #assert_equal( false, nil | false )

end
