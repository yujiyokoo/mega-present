def draw_text(str, x, y)
  MegaMrbc.draw_text(str, x, y)
end

def joypad_state(pad_num)
  MegaMrbc.read_joypad(pad_num)
end

def wait_vblank
  MegaMrbc.wait_vblank
end

def wait_start
  prev = joypad_state(0)
  state = 0
  while true do
    MegaMrbc.call_rand # help random seem more random
    state = joypad_state(0)
    if (state & 0x80 & ~prev) != 0
      wait_vblank
      break
    end
    prev = state
    wait_vblank
  end
end

def wait_cmd
  prev = joypad_state(0)
  state = 0
  while true do
    MegaMrbc.call_rand # help random seem more random
    state = joypad_state(0)
    if (state & 0x80 & ~prev) != 0 # start
      MegaMrbc.klog("0x80")
      wait_vblank
      return :fwd
    elsif (state & 0x40 & ~prev) != 0 # a
      MegaMrbc.klog("0x40")
      wait_vblank
      return :fwd
    elsif (state & 0x10 & ~prev) != 0 # b
      MegaMrbc.klog("0x10")
      wait_vblank
      return :back
    end
    # also c is 0x20
    prev = state
    wait_vblank
  end
end

class Page
  def initialize(content)
    @content = content
  end

  def render
    return if @content.nil?
    @content.split("\n").each do |line|
      next if line[0] == '#'

      if line.start_with? "-title:"
        title = line.split("-title:")[1].strip
        text_centre(title, 0)
      elsif line.start_with? "-txt,"
        cmd = line.split(":")[0].split(",")
        x = cmd[1].to_i
        y = cmd[2].to_i
        bg = cmd[3]
        idx = 0
        idx += 1 while(line[idx] != ":") # FIXME: this will break if ':' is not present
        content = line.slice!(idx + 1, line.length).strip
        MegaMrbc.draw_bg(content, bg[2].to_i, x, y) if bg != nil
        draw_text(content, x, y)
      elsif line.start_with? "-setcolour,"
        cmd = line.split(":")[0].split(",")
        colour_id = cmd[1]
        colour_val = cmd[2]
        MegaMrbc.set_pal_colour(colour_id, colour_val)
      elsif line.start_with? "-txtpal,"
        cmd = line.split(":")[0].split(",")
        pal = cmd[1]
        draw_text("setting pal: #{pal}", 1, 27)
        MegaMrbc.set_txt_pal(pal)
      elsif line.start_with? "-pause:"
        wait_cmd # do not care about which button
      elsif line.start_with? "-image,"
        cmd = line.split(":")[0].split(",")
        MegaMrbc.draw_image(cmd[1].to_i, cmd[2].to_i, cmd[3]) # x, y, image name
      end
    end
  end

  def text_centre(txt, row)
    draw_text(txt, 20 - (txt.length / 2), row)
  end
end

class Presentation
  def self.start
    MegaMrbc.test_func
    wait_start_with_message
    self.new.main_loop
  end

  def main_loop
    running = true
    cmd = :fwd
    while running do
      MegaMrbc.clear_screen
      wait_vblank
      if cmd == :fwd
        page = next_page
      elsif cmd == :back
        page = prev_page
      end
      page.render

      cmd = wait_cmd
    end
  end

  def next_page
    @pages ||= MegaMrbc.read_content.split("\n=")
    @index ||= -1
    @index += 1
    @page = @pages[@index]
    return Page.new(@page)
  end

  def prev_page
    @pages ||= MegaMrbc.read_content.split("\n=")
    @index ||= 0
    @index -= 1 unless @index < 1
    @page = @pages[@index]
    return Page.new(@page)
  end

  def wait_start_with_message
    state = 0
    count = 0
    # draw_text("MegaRuby-Present", 12, 10)
    while true do
      MegaMrbc.scroll_one_step
      MegaMrbc.call_rand # help random seem more random
      if count / 30 >= 1 # switch every 0.5s
        draw_text("Press start", 14, 18)
      else
        draw_text("           ", 14, 18)
      end
      state = joypad_state(0)
      break if (state & 0x80) != 0
      count = 0 if count > 60 # count resets at 60
      count += 1
      wait_vblank
    end
  end
end

Presentation.start
