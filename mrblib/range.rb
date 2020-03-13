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
