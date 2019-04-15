# frozen_string_literal: true

class SprintfTest < MrubycTestCase

  description "%c"
  def c_case
    assert_equal "A", sprintf("%c", 65)
    assert_equal "A", sprintf("%c", "A")

    # width
    assert_equal "  A", sprintf("%3c", 65)

    # flag '-'
    assert_equal "A  ", sprintf("%-3c", 65)
    assert_equal "A", sprintf("%-c", 65)
  end

  description "%s"
  def s_case
    assert_equal "ABC", sprintf("%s", "ABC")

    # width
    assert_equal "  ABC", sprintf("%5s", "ABC")
    assert_equal "ABCDE", sprintf("%5s", "ABCDE")
    assert_equal "ABCDEF", sprintf("%5s", "ABCDEF")

    # precision
    assert_equal "ABCDE", sprintf("%.5s", "ABCDEF")
    assert_equal "ABC", sprintf("%.5s", "ABC")

    # flag '-'
    assert_equal "ABC  ", sprintf("%-5s", "ABC")
    assert_equal "ABC", sprintf("%-s", "ABC")

    # width, precision, flag '-'
    assert_equal "  ABC", sprintf("%5.5s", "ABC")
    assert_equal "ABC  ", sprintf("%-5.5s", "ABC")
    assert_equal "ABCDE", sprintf("%5.5s", "ABCDEFG")
    assert_equal "     ABCDE", sprintf("%10.5s", "ABCDEFG")
    assert_equal "ABCDE     ", sprintf("%-10.5s", "ABCDEFG")
  end

  description "%d, %i"
  def d_i_case
    assert_equal "123", sprintf("%d", 123)
    assert_equal "123", sprintf("%i", 123)
    assert_equal "-123", sprintf("%d", -123)

    # width
    assert_equal "  123", sprintf("%5d", 123)
    assert_equal "12345", sprintf("%5d", 12345)
    assert_equal "123456", sprintf("%5d", 123456)
    assert_equal " -123", sprintf("%5d", -123)
    assert_equal "-1234", sprintf("%5d", -1234)
    assert_equal "-12345", sprintf("%5d", -12345)

    # precision
    assert_equal "   01", sprintf("%5.2d", 1)
    assert_equal "  123", sprintf("%5.2d", 123)
    assert_equal "00123",  sprintf("%5.5d", 123)
    assert_equal "12345",  sprintf("%5.5d", 12345)
    assert_equal "123456", sprintf("%5.5d", 123456)
    assert_equal "00001",  sprintf("%3.5d", 1)
    assert_equal "12345",  sprintf("%3.5d", 12345)
    assert_equal "123456", sprintf("%3.5d", 123456)

    assert_equal "  -01", sprintf("%5.2d", -1)
    assert_equal "  -12", sprintf("%5.2d", -12)
    assert_equal " -123", sprintf("%5.2d", -123)

    # flag '+'
    assert_equal "+1", sprintf("%+d", 1)
    assert_equal "-1", sprintf("%+d", -1)
    assert_equal "+0", sprintf("%+d", 0)
    assert_equal "+12", sprintf("%+3d", 12)
    assert_equal "+123", sprintf("%+3d", 123)

    assert_equal "  +01", sprintf("%+5.2d", 1)
    assert_equal "  -01", sprintf("%+5.2d", -1)

    # flag ' '
    assert_equal " 1", sprintf("% d", 1)
    assert_equal "-1", sprintf("% d", -1)
    assert_equal " 0", sprintf("% d", 0)
    assert_equal " 12",  sprintf("% 3d",   12)
    assert_equal " 123", sprintf("% 3d",  123)
    assert_equal "-12",  sprintf("% 3d",  -12)
    assert_equal "-123", sprintf("% 3d", -123)

    # flag '0'
    assert_equal "001", sprintf("%03d", 1)
    assert_equal "123", sprintf("%03d", 123)
    assert_equal "1234", sprintf("%03d", 1234)
  end

  description "%u"
  def u_case
    assert_equal "-1", sprintf("%u", -1)
  end

  description "%x, %X"
  def x_X_case
    assert_equal "0", sprintf("%x", 0)
    assert_equal "a", sprintf("%x", 10)
    assert_equal "A", sprintf("%X", 10)

    assert_equal "..F",     sprintf("%X", -1)
    assert_equal "..FB2E",  sprintf("%X", -1234)
    assert_equal "..FCFC7", sprintf("%X", -12345)

    # width
    assert_equal "   0",  sprintf("%4X", 0)
    assert_equal "   A",  sprintf("%4X", 10)
    assert_equal "FFFF",  sprintf("%4X", 0xffff)
    assert_equal "10000", sprintf("%4X", 0x10000)

    assert_equal " ..f",  sprintf("%4x", -1)
    assert_equal "..f4",  sprintf("%4x", -12)
    assert_equal "..f85", sprintf("%4x", -123)

    # flag '-'
    assert_equal "0   ",  sprintf("%-4x", 0)
    assert_equal "1   ",  sprintf("%-4x", 1)
    assert_equal "FFFF",  sprintf("%-4X", 0xffff)
    assert_equal "10000", sprintf("%-4X", 0x10000)

    assert_equal "..f ",  sprintf("%-4x", -1)
    assert_equal "..f4",  sprintf("%-4x", -12)
    assert_equal "..f85", sprintf("%-4x", -123)

    # flag '0'
    assert_equal "0000",  sprintf("%04X", 0x0)
    assert_equal "0010",  sprintf("%04X", 0x10)
    assert_equal "1111",  sprintf("%04X", 0x1111)
    assert_equal "10000", sprintf("%04X", 0x10000)

    # no compatibility with cruby and mruby.
    assert_equal "ffff", sprintf("%04x", -1)
    assert_equal "fff4", sprintf("%04x", -12)
    assert_equal "ff85", sprintf("%04x", -123)
    assert_equal "fb2e", sprintf("%04x", -1234)
    assert_equal "cfc7", sprintf("%04x", -12345)
  end

  description "%b, %B"
  def b_B_case
    assert_equal   "0", sprintf("%b", 0)
    assert_equal   "1", sprintf("%b", 1)
    assert_equal  "10", sprintf("%b", 2)
    assert_equal  "11", sprintf("%b", 3)
    assert_equal "100", sprintf("%b", 4)
    assert_equal "101", sprintf("%b", 5)
    assert_equal   "1", sprintf("%B", 1)

    assert_equal    "..1", sprintf("%b", -1)
    assert_equal   "..10", sprintf("%b", -2)
    assert_equal  "..101", sprintf("%b", -3)
    assert_equal  "..100", sprintf("%b", -4)
    assert_equal "..1011", sprintf("%b", -5)

    # width
    assert_equal "   0", sprintf("%4b", 0)
    assert_equal "   1", sprintf("%4b", 1)
    assert_equal "1111", sprintf("%4b", 15)
    assert_equal "10000", sprintf("%4b", 16)

    assert_equal " ..1", sprintf("%4b", -1)
    assert_equal "..10", sprintf("%4b", -2)
    assert_equal "..100", sprintf("%4b", -4)

    # flag '-'
    assert_equal "0   ", sprintf("%-4b", 0)
    assert_equal "1   ", sprintf("%-4b", 1)
    assert_equal "1111", sprintf("%-4b", 15)
    assert_equal "10000", sprintf("%-4b", 16)

    assert_equal "..1 ", sprintf("%-4b", -1)
    assert_equal "..10", sprintf("%-4b", -2)
    assert_equal "..100", sprintf("%-4b", -4)

    # flag '0'
    assert_equal "0000",  sprintf("%04b", 0)
    assert_equal "0001",  sprintf("%04b", 1)
    assert_equal "1111",  sprintf("%04b", 15)
    assert_equal "10000", sprintf("%04b", 16)

    # no compatibility with cruby and mruby.
    assert_equal "1111", sprintf("%04b", -1)
    assert_equal "1000", sprintf("%04b", -8)
    assert_equal "0111", sprintf("%04b", -9)
  end
end
