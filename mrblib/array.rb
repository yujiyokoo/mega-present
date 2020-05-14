#
# Array, mrubyc class library
#
#  Copyright (C) 2015-2018 Kyushu Institute of Technology.
#  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

class Array

  #
  # each
  #
  def each
    idx = 0
    while idx < length
      yield self[idx]
      idx += 1
    end
    self
  end

  #
  # each index
  #
  def each_index
    idx = 0
    while idx < length
      yield idx
      idx += 1
    end
    self
  end

  #
  # each with index
  #
  def each_with_index
    idx = 0
    while idx < length
      yield self[idx], idx
      idx += 1
    end
    self
  end

  #
  # collect
  #
  def collect
    idx = 0
    ary = []
    while idx < length
      ary[idx] = yield self[idx]
      idx += 1
    end
    ary
  end

  #
  # collect!
  #
  def collect!
    idx = 0
    while idx < length
      self[idx] = yield self[idx]
      idx += 1
    end
    self
  end

  #
  # sort!
  #
  def sort!(&block)
    n = self.size - 1
    i = 0
    while i < n
      j = i
      while j < n
        j += 1
        v_i = self[i]
        v_j = self[j]
        if block
          next if block.call(v_i, v_j) < 0
        else
          next if v_i < v_j
        end
        self[i] = v_j
        self[j] = v_i
      end
      i += 1
    end
    self
  end

  #
  # sort
  #
  def sort(&block)
    self.dup.sort!(&block)
  end
end
