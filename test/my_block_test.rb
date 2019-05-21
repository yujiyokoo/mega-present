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

end
