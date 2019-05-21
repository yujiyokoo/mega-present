# frozen_string_literal: true

class MathTest < MrubycTestCase

  description 'Math class'
  def all
    # Math class methods
    assert_in_delta 1.0471, Math.acos(0.5)
    assert_in_delta 0.9624, Math.acosh(1.5)
    assert_in_delta 0.5235, Math.asin(0.5)
    assert_in_delta 0.4812, Math.asinh(0.5)
    assert_in_delta 0.4636, Math.atan(0.5)
    assert_in_delta 0.3805, Math.atan2(0.2, 0.5)
    assert_in_delta 0.5493, Math.atanh(0.5)
    assert_in_delta 0.7937, Math.cbrt(0.5)
    assert_in_delta 0.8776, Math.cos(0.5)
    assert_in_delta 1.1276, Math.cosh(0.5)
    assert_in_delta 0.5205, Math.erf(0.5)
    assert_in_delta 0.4795, Math.erfc(0.5)
    assert_in_delta 1.6487, Math.exp(0.5)
    assert_in_delta 5.0, Math.hypot(3, 4)
    assert_in_delta 12.8, Math.ldexp(3.2, 2)
    assert_in_delta 1.0986, Math.log(3)
    assert_in_delta 0.4771, Math.log10(3)
    assert_in_delta 1.5849, Math.log2(3)
    assert_in_delta 0.4794, Math.sin(0.5)
    assert_in_delta 0.5210, Math.sinh(0.5)
    assert_in_delta 0.7071, Math.sqrt(0.5)
    assert_in_delta 0.5463, Math.tan(0.5)
    assert_in_delta 0.4621, Math.tanh(0.5)
  end
end
