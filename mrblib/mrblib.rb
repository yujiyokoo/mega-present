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
class Range

  # each
  def each 
    val = self.first
    last = self.last

    lim = last
    lim += 1 unless exclude_end?
    i = val
    while i < lim do
      yield i
      i += 1
    end
    return self
  end

end
