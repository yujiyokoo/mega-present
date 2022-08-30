def draw_text(str, x, y)
  MegaMrbc.draw_text(str, x, y)
end

def joypad_state(pad_num)
  MegaMrbc.read_joypad(pad_num)
end

class Page
  def initialize(content, presentation)
    @content = content
    @presentation = presentation
  end

  def render
    return if @content.nil?
    @content.split("\n").each do |line|
      next if line[0] == '#'

      if line.start_with? "-title:"
        @curr_mode = nil
        title = line.split("-title:")[1].strip
        text_centre(title, 0)
      elsif line.start_with? "-txt,"
        @curr_mode = :text
        cmd = line.split(":")[0].split(",")
        @x = cmd[1].to_i
        @y = cmd[2].to_i
        bg = cmd[3]
        idx = 0
        idx += 1 while(line[idx] != ":") # FIXME: this will break if ':' is not present
        content = line.slice!(idx + 1, line.length)
        MegaMrbc.draw_bg(content, bg[2].to_i, @x, @y) if bg != nil
        draw_text(content, @x, @y)
      elsif line.start_with? "-setcolour,"
        @curr_mode = nil
        cmd = line.split(":")[0].split(",")
        colour_id = cmd[1].to_i
        colour_val = cmd[2].to_i(16)
        MegaMrbc.klog("setting colour #{colour_id} to #{colour_val}")
        MegaMrbc.set_pal_colour(colour_id, colour_val)
      elsif line.start_with? "-txtpal,"
        @curr_mode = nil
        cmd = line.split(":")[0].split(",")
        pal = cmd[1]
        draw_text("setting pal: #{pal}", 1, 27)
        MegaMrbc.set_txt_pal(pal)
      elsif line.start_with? "-pause:"
        @curr_mode = nil
        @presentation.wait_cmd # do not care about which button
      elsif line.start_with? "-image,"
        @curr_mode = nil
        cmd = line.split(":")[0].split(",")
        MegaMrbc.klog("drawing " + cmd[3])
        MegaMrbc.draw_image(cmd[1].to_i, cmd[2].to_i, cmd[3]) # x, y, image name
      elsif line.start_with? "-code,"
        @curr_mode = :code
        cmd = line.split(":")[0].split(",")
        @x = cmd[1].to_i
        @y = cmd[2].to_i
        idx = 0
        idx += 1 while(line[idx] != ":") # FIXME: this will break if ':' is not present
        code = line.slice!(idx + 1, line.length)
        render_code_line(code, @x, @y)
      elsif line.start_with? "-rect,"
        cmd = line.split(":")[0].split(",")
        x = cmd[1].to_i
        y = cmd[2].to_i
        w = cmd[3].to_i
        h = cmd[4].to_i
        bg_pal = cmd[5] ? cmd[5].to_i : nil
        render_rect(x, y, w, h, bg_pal)
      elsif line.start_with? "-arrow,"
        cmd = line.split(":")[0].split(",")
        x = cmd[1].to_i
        y = cmd[2].to_i
        dir = cmd[3]
        len = cmd[4].to_i
        draw_arrow(x, y, dir, len, false)
      elsif line.start_with? "-tarrow,"
        cmd = line.split(":")[0].split(",")
        x = cmd[1].to_i
        y = cmd[2].to_i
        dir = cmd[3]
        len = cmd[4].to_i
        draw_arrow(x, y, dir, len, true)
      elsif line.start_with? "-bgcol,"
        cmd = line.split(":")[0].split(",")
        bgnum = cmd[1].to_i(16)
        MegaMrbc.klog("bgnum is #{bgnum}")
        MegaMrbc.set_bg_num(bgnum)
      elsif line.start_with? "-sleep_raw,"
        cmd = line.split(":")[0].split(",")
        len = cmd[1].to_i
        MegaMrbc.sleep_raw(len)
      elsif @curr_mode == :text
        draw_text(line, @x, @y+=1)
      elsif @curr_mode == :code
        render_code_line(line, @x, @y+=1)
      end
    end
  end

  def draw_t_horzontal(curr_x, x, y, direction, start_t)
    if curr_x == x && start_t && direction == 'r'
      MegaMrbc.draw_right_t(curr_x, y)
    elsif curr_x == x && start_t && direction == 'l'
      MegaMrbc.draw_left_t(curr_x, y)
    else
      MegaMrbc.draw_horizontal(curr_x, y)
    end
  end

  def draw_t_vertical(curr_y, x, y, direction, start_t)
    if curr_y == y && start_t && direction == 'd'
      MegaMrbc.draw_upright_t(x, curr_y)
    elsif curr_y == y && start_t && direction == 'u'
      MegaMrbc.draw_flipped_t(x, curr_y)
    else
      MegaMrbc.draw_vertical(x, curr_y)
    end
  end

  def draw_arrow(x, y, direction, length, start_t)
    MegaMrbc.klog("x, y, direction: #{x}, #{y}, #{direction}")
    if direction == 'r'
      i = x
      while(i < x + length - 1)
        draw_t_horzontal(i, x, y, direction, start_t)
        i += 1
      end
      MegaMrbc.draw_arrow_r(i, y)
    elsif direction == 'l'
      i = x
      while(i > x - (length - 1))
        draw_t_horzontal(i, x, y, direction, start_t)
        i -= 1
      end
      MegaMrbc.draw_arrow_l(i, y)
    elsif direction == 'u'
      i = y
      while(i > y - (length - 1))
        MegaMrbc.klog("drawing on #{x}, #{i}")
        #MegaMrbc.draw_vertical(x, i)
        draw_t_vertical(i, x, y, direction, start_t)
        i -= 1
      end
      MegaMrbc.draw_arrow_u(x, i)
    elsif direction == 'd'
      i = y
      while(i < y + length - 1)
        MegaMrbc.klog("drawing on #{x}, #{i}")
        #MegaMrbc.draw_vertical(x, i)
        draw_t_vertical(i, x, y, direction, start_t)
        i += 1
      end
      MegaMrbc.draw_arrow_d(x, i)
    end


  end

  def render_rect(x, y, w, h, bg_pal = nil)
    curr_x = x + 1
    while(curr_x < x + w)
      MegaMrbc.draw_horizontal(curr_x, y)
      MegaMrbc.draw_horizontal(curr_x, y+h)
      if bg_pal
        MegaMrbc.set_bg_colour(curr_x, y, bg_pal, :bottom)
        MegaMrbc.set_bg_colour(curr_x, y+h, bg_pal, :top)
      end
      curr_x += 1
    end
    curr_y = y + 1
    while(curr_y < y + h)
      MegaMrbc.draw_vertical(x, curr_y)
      MegaMrbc.draw_vertical(x+w, curr_y)
      if bg_pal
        MegaMrbc.set_bg_colour(x, curr_y, bg_pal, :right)
        fill_x = x + 1
        while(fill_x < x+w)
          MegaMrbc.set_bg_colour(fill_x, curr_y, bg_pal, :full)
          fill_x += 1
        end
        MegaMrbc.set_bg_colour(x+w, curr_y, bg_pal, :left)
      end
      curr_y += 1
    end

    # corners
    MegaMrbc.draw_top_left(x, y)
    MegaMrbc.draw_top_right(x+w, y)
    MegaMrbc.draw_bottom_left(x, y+h)
    MegaMrbc.draw_bottom_right(x+w, y+h)

    if bg_pal
      MegaMrbc.set_bg_colour(x, y, bg_pal, :top_left)
      MegaMrbc.set_bg_colour(x+w, y, bg_pal, :top_right)
      MegaMrbc.set_bg_colour(x, y+h, bg_pal, :bottom_left)
      MegaMrbc.set_bg_colour(x+w, y+h, bg_pal, :bottom_right)
    end
  end

  def render_code_line(txt, x, y)
    fragments = txt.split('"')
    # palettes
    # 0 => white
    curr_offset = 0
    fragments.each_with_index { |f, i|
      # MegaMrbc.klog("f is '#{f}'")
      if i % 2 == 0
        terms = break_line(f)
        terms.each { |t|
          # MegaMrbc.klog("t is '#{t}'")
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
        # MegaMrbc.klog(term)
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
        page = next_page unless @index >= (@pages.size - 1)
      elsif cmd == :back
        page = prev_page
      end
      page.render
      wait_vblank

      cmd = wait_cmd
    end
  end

  def next_page
    @index ||= -1
    @index += 1
    @page = @pages[@index]
    return Page.new(@page, self)
  end

  def prev_page
    @index ||= 0
    @index -= 1 unless @index < 1
    @page = @pages[@index]
    return Page.new(@page, self)
  end

  def wait_cmd
    prev = joypad_state(0)
    state = 0
    returning = nil
    while true do
      MegaMrbc.call_rand # help random seem more random
      state = joypad_state(0)
      if (state & 0x80 & ~prev) != 0 # start
        returning = :fwd
      elsif (state & 0x40 & ~prev) != 0 # a
        returning = :fwd
      elsif (state & 0x10 & ~prev) != 0 # b
        returning = :back
      end
      # also c is 0x20
      prev = state
      wait_vblank
      break if returning
    end
    return returning
  end

  def wait_vblank(show_timer = true)
    if show_timer
      MegaMrbc.show_progress(@index, @pages.size) if @index && @pages
      MegaMrbc.show_timer
    else
      MegaMrbc.hide_progress
      MegaMrbc.hide_timer
    end
    MegaMrbc.wait_vblank
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
      wait_vblank(false)
    end
    MegaMrbc.play_se
  end
end

Presentation.start
