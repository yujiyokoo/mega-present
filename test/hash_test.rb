# frozen_string_literal: true

class HashTest < MrubycTestCase

  description "生成"
  def constractor_case
    h = {a: 1, b: 2}
    assert_equal( {:a=>1, :b=>2 }, h )

    h = Hash.new
    assert_equal( {}, h )
  end

  description "値の取り出し"
  def pick_value_case
    h = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    assert_equal( "value", h[:key] )
    assert_equal( :value,  h["key"] )
    assert_equal( "Three", h[3] )
    assert_equal( 444,     h[:"4"] )
  end

  description "値の設定、サイズ"
  def setter_case
    h = Hash.new
    assert_equal( 0, h.size )
    h["key"] = :value
    assert_equal( :value,  h["key"] )
    assert_equal( 1, h.size )
  end

  description "存在しないキーにアクセスした場合nilが返ること"
  def nonexistent_case
    h = Hash.new
    assert_equal( nil, h["no-exist-key"] )
  end

  description "operator !="
  def negate_op_case
    h = Hash.new
    h1 = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    assert_not_equal( h, h1 )
  end

  description "==  (順序が違うHash同士の比較)"
  def eq_eq_case
    h1 = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    h = Hash.new
    h["key"] = :value
    h[:key] = "value"
    h[:"4"] = 444
    h[3] = "Three"
    assert_equal( h, h1 )

    assert_equal( 4, h.size )
    assert_equal( 4, h1.size )
  end

  description "値の上書き"
  def override_case
    h = Hash.new
    h["key"] = :value
    h[:"4"] = 444
    h[3] = "Three"
    h1 = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    h[:key] = "other value"
    assert_not_equal( h, h1 )
    assert_equal( 4, h.size )
  end

  description "clear"
  def clear_case
    h = {}
    h[:key] = "other value"
    h.clear
    assert_equal( {}, h )
    assert_equal( 0, h.size )
  end

  description "dup"
  def dup_case
    h = {:a=>"A", :b=>"B"}
    h1 = h
    assert_equal( h, h1 )

    h1[:a] = "AA"
    assert_equal( h, h1 )

    h1 = h.dup
    h1[:b] = "BB"
    assert_not_equal( h, h1 )
  end

  description "delete"
  def delete_case
    h = {:ab => "some", :cd => "all"}

    assert_equal( "some", h.delete(:ab) )
    assert_equal( h, {:cd=>"all"} )

    assert_equal( nil, h.delete(:ef) )

    # TODO delete with block
  end

  description "empty?"
  def is_empty_case
    assert_equal( true, {}.empty? )
    assert_equal( false, {:a=>1}.empty? )
  end

  description "has_key?"
  def has_key_case
    h = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    assert_equal( true,  h.has_key?(:key) )
    assert_equal( false, h.has_key?(:key2) )
  end

  description "has_value?"
  def has_value_case
    h = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    assert_equal( true,  h.has_value?(444) )
    assert_equal( false, h.has_value?(555) )
  end

  description "key"
  def key_case
    h = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    assert_equal( "key", h.key(:value) )
    assert_equal( nil,   h.key(:no_exist) )
  end

  description "keys"
  def keys_case
    h = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    assert_equal( [:key, "key", 3, :"4"], h.keys )
  end

  description "values"
  def values_case
    h = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    assert_equal( ["value", :value, "Three", 444], h.values )
  end

  description "size, length, count"
  def size_case
    h = {:key=>"value", "key"=>:value, 3=>"Three", :"4"=>444}
    assert_equal( 4, h.size )
    assert_equal( 4, h.length )
    assert_equal( 4, h.count )
  end

  description "merge"
  def merge_case
    foo = {1=>"a", 2=>"b", 3=>"c"}
    bar = {2=>"B", 3=>"C", 4=>"D"}
    assert_equal( {1=>"a", 2=>"B", 3=>"C", 4=>"D"}, foo.merge(bar) )
    assert_equal( {1=>"a", 2=>"b", 3=>"c"}, foo )
    assert_equal( {2=>"B", 3=>"C", 4=>"D"}, bar )
  end

  description "merge!"
  def merge_bung_case
    foo = {1=>"a", 2=>"b", 3=>"c"}
    bar = {2=>"B", 3=>"C", 4=>"D"}
    assert_equal( {1=>"a", 2=>"B", 3=>"C", 4=>"D"}, foo.merge!(bar) )
    assert_equal( {1=>"a", 2=>"B", 3=>"C", 4=>"D"}, foo )
    assert_equal( {2=>"B", 3=>"C", 4=>"D"}, bar )
  end

  description "to_h"
  def to_h_case
    h = {}
    assert_equal( {}, h.to_h )
  end

end
