#
# Integer, mrubyc class library
#
#  Copyright (C) 2015-2021 Kyushu Institute of Technology.
#  Copyright (C) 2015-2021 Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

class Integer

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
