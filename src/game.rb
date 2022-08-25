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
        @is_code = false
        title = line.split("-title:")[1].strip
        text_centre(title, 0)
      elsif line.start_with? "-txt,"
        @is_code = false
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
        @is_code = false
        cmd = line.split(":")[0].split(",")
        colour_id = cmd[1]
        colour_val = cmd[2]
        MegaMrbc.set_pal_colour(colour_id, colour_val.to_i(16))
      elsif line.start_with? "-txtpal,"
        @is_code = false
        cmd = line.split(":")[0].split(",")
        pal = cmd[1]
        draw_text("setting pal: #{pal}", 1, 27)
        MegaMrbc.set_txt_pal(pal)
      elsif line.start_with? "-pause:"
        @is_code = false
        wait_cmd # do not care about which button
      elsif line.start_with? "-image,"
        @is_code = false
        cmd = line.split(":")[0].split(",")
        MegaMrbc.draw_image(cmd[1].to_i, cmd[2].to_i, cmd[3]) # x, y, image name
      elsif line.start_with? "-code,"
        @is_code = true
        cmd = line.split(":")[0].split(",")
        @x = cmd[1].to_i
        @y = cmd[2].to_i
        idx = 0
        idx += 1 while(line[idx] != ":") # FIXME: this will break if ':' is not present
        code = line.slice!(idx + 1, line.length)
        render_code_line(code, @x, @y)
      elsif @is_code
        render_code_line(line, @x, @y+=1)
      end
    end
  end

  def render_code_line(txt, x, y)
    fragments = txt.split('"')
    # palettes
    # 0 => white
    curr_offset = 0
    fragments.each_with_index { |f, i|
      MegaMrbc.klog("f is '#{f}'")
      if i % 2 == 0
        terms = break_line(f)
        terms.each { |t|
          MegaMrbc.klog("t is '#{t}'")
          render_term(t, x + curr_offset, y)
          curr_offset += t.length
        }
        #MegaMrbc.set_txt_pal(0)
        #draw_text(f, x + curr_offset, y)
        #curr_offset += f.length
      else
        MegaMrbc.set_txt_pal(2)
        draw_text('"', x + curr_offset, y)
        draw_text(f, x + curr_offset + 1, y)
        draw_text('"', x + curr_offset + 1 + f.length, y)
        curr_offset += f.length + 2
      end
    }
  end

  def break_line(line)
    terms = []
    start_i = i = 0
    curr_mode = if line[i] == ' '
      :space
    else
      :word
    end

    while i < line.length
      if (curr_mode == :space && line[i] != ' ') || (curr_mode == :word && line[i] == ' ') || i == line.length-1
        i += 1 if i == line.length-1 # special handling for end of line
        term = line[start_i, i-start_i]
        MegaMrbc.klog(term)
        terms << term
        start_i = i
        curr_mode = if curr_mode == :space
          :word
        elsif curr_mode == :word
          :space
        end
      end
      i += 1
    end

    terms
  end

  def render_term(term, x, y)
    if "ABCDEFGHIJKLMNOPQRSTUVWXYZ".include?(term[0])
      MegaMrbc.set_txt_pal(1)
    elsif ["do", "end", "if", "unless", "else", "elsif", "while", "for", "class", "def"].include?(term)
      MegaMrbc.set_txt_pal(3)
    else
      MegaMrbc.set_txt_pal(0)
    end

    draw_text(term, x, y)
  end

  def text_centre(txt, row)
    draw_text(txt, 20 - (txt.length / 2), row)
  end
end

class Presentation
  def self.start
    self.new.begin_presentation
  end

  def initialize
    @pages ||= MegaMrbc.read_content.split("\n=")
    MegaMrbc.show_progress(0, @pages.size)
  end

  def begin_presentation
    MegaMrbc.test_func
    wait_start_with_message
    main_loop
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
      MegaMrbc.show_timer()
    end
  end

  def next_page
    @index ||= -1
    @index += 1
    @page = @pages[@index]
    MegaMrbc.show_progress(@index, @pages.size)
    return Page.new(@page)
  end

  def prev_page
    @index ||= 0
    @index -= 1 unless @index < 1
    @page = @pages[@index]
    MegaMrbc.show_progress(@index, @pages.size)
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
