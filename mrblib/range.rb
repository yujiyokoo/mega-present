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
