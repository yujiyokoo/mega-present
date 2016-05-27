class Aut

def initialize(maxx)
  @maxx = maxx
end

def init_color(x)
  if x == @max / 2 then return 1 else return 0 end
end

def color(a,b,c)
  if a == 0 && b == 0 && c == 0 then return 0 end
  if a == 0 && b == 0 && c == 1 then return 1 end
  if a == 0 && b == 1 && c == 0 then return 0 end
  if a == 0 && b == 1 && c == 1 then return 1 end
  if a == 1 && b == 0 && c == 0 then return 1 end
  if a == 1 && b == 0 && c == 1 then return 0 end
  if a == 1 && b == 1 && c == 0 then return 1 end
  if a == 1 && b == 1 && c == 1 then return 0 end
end

end #class
