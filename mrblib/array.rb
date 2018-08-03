#
# Array, mrubyc class library
#
#  Copyright (C) 2015-2018 Kyushu Institute of Technology.
#  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#
#  Memory management for objects in mruby/c.
#
#
class Array

  # each
  def each
    idx = 0
    while idx < length
      yield self[idx]
      idx += 1
    end
    self
  end

  # collect
  def collect
    idx = 0
    ary = []
    while idx < length
      ary[idx] = yield self[idx]
      idx += 1
    end
    ary
  end

end
