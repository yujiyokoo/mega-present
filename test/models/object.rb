class Object
  def foo
    return "foo"
  end

  alias :_orig_foo :foo

  def foo
    return _orig_foo * 2
  end
end
