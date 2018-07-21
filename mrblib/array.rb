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
