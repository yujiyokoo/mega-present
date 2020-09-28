#
# Array, mrubyc class library
#
#  Copyright (C) 2015-2020 Kyushu Institute of Technology.
#  Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

class Array

  #
  # collect
  #
  def collect
    i = 0
    ary = []
    while i < length
      ary[i] = yield self[i]
      i += 1
    end
    return ary
  end
  alias map collect

  #
  # collect!
  #
  def collect!
    i = 0
    while i < length
      self[i] = yield self[i]
      i += 1
    end
    return self
  end
  alias map! collect!

  #
  # delete_if
  #
  def delete_if
    i = 0
    while i < length
      if yield self[i]
        delete_at(i)
      else
        i += 1
      end
    end
    return self
  end

  #
  # each
  #
  def each
    i = 0
    while i < length
      yield self[i]
      i += 1
    end
    return self
  end

  #
  # each index
  #
  def each_index
    i = 0
    while i < length
      yield i
      i += 1
    end
    return self
  end

  #
  # each with index
  #
  def each_with_index
    i = 0
    while i < length
      yield self[i], i
      i += 1
    end
    return self
  end

  #
  # reject!
  #
  def reject!( &block )
    n = length
    delete_if( &block )
    return n == length ? nil : self
  end

  #
  # reject
  #
  def reject( &block )
    return self.dup.delete_if( &block )
  end

  #
  # sort!
  #
  def sort!( &block )
    n = length - 1
    i = 0
    while i < n
      j = i
      while j < n
        j += 1
        v_i = self[i]
        v_j = self[j]
        if block
          next if block.call(v_i, v_j) <= 0
        else
          next if v_i <= v_j
        end
        self[i] = v_j
        self[j] = v_i
      end
      i += 1
    end
    return self
  end

  #
  # sort
  #
  def sort( &block )
    return self.dup.sort!( &block )
  end

end
#
# Object, mrubyc class library
#
#  Copyright (C) 2015-2020 Kyushu Institute of Technology.
#  Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

RUBY_VERSION = "1.9"
MRUBYC_VERSION = "3.0"
#
# Hash, mrubyc class library
#
#  Copyright (C) 2015-2019 Kyushu Institute of Technology.
#  Copyright (C) 2015-2019 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

class Hash

  #
  # each
  #
  def each
    keys = self.keys
    len = length
    i = 0
    while i < len
      key = keys[i]
      yield( key, self[key] )
      i += 1
    end

    return self
  end

end
#
# Fixnum, mrubyc class library
#
#  Copyright (C) 2015-2018 Kyushu Institute of Technology.
#  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

class Fixnum

  # times
  def times
    i = 0
    while i < self
      yield i
      i += 1
    end
    self
  end

end
#
# Object, mrubyc class library
#
#  Copyright (C) 2015-2020 Kyushu Institute of Technology.
#  Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

class Object

  # loop
  def loop
    while true
      yield
    end
  end

end
#
# Range, mrubyc class library
#
#  Copyright (C) 2015-2018 Kyushu Institute of Technology.
#  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

class Range

  # each
  def each
    lim = self.last
    lim += 1 unless exclude_end?
    i = self.first
    while i < lim do
      yield i
      i += 1
    end
    return self
  end

end
#
# String, mrubyc class library
#
#  Copyright (C) 2015-2018 Kyushu Institute of Technology.
#  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

class String

  ##
  # Passes each byte in str to the given block.
  #
  def each_byte
    idx = 0
    while idx < length
      yield self[idx].ord
      idx += 1
    end
    self
  end

  ##
  # Passes each character in str to the given block.
  #
  def each_char
    idx = 0
    while idx < length
      yield self[idx]
      idx += 1
    end
    self
  end

end
