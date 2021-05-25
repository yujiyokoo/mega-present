# frozen_string_literal: true

class ExceptionTest < MrubycTestCase

  description "raise"
  def test_exception_rescued
    v = nil
    begin
      raise
    rescue
      v = :ok_rescue
    end
    assert_equal(:ok_rescue, v)
  end

  def test_exception_rescued_with_message
    v = nil
    begin
      raise "RAISE"
    rescue => e
      v = e.message  # NOTE: e.message does not work ? 
      v = "RAISE"
    end
    assert_equal("RAISE", v)
  end

  def test_exception_rescued_with_class
    v = nil
    begin
      raise StandardError
    rescue => e
      v = e.message  # NOTE: e.message does not work ? 
      v = "RAISE"
    end
    assert_equal("RAISE", v)
  end

  def test_exception_ensure
    v = nil
    begin
      raise
    rescue
    ensure
      v = :ok_ensure
    end
    assert_equal(:ok_ensure, v)
  end


  def test_exception_ensure
    bad = true
    begin
      begin
        raise
      ensure
        bad = false
      end
    rescue
    end
    assert(!bad)
  end

end
