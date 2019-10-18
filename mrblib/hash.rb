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
