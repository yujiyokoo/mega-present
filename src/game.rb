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
        title = line.split("-title:")[1]
        MegaMrbc.draw_text(title, 2, 0)
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
        MegaMrbc.draw_text(content, @x, @y)
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
        MegaMrbc.set_txt_pal(pal)
      elsif line.start_with? "-pz:"
        @curr_mode = nil
        if @presentation.wait_cmd == :reload
          return :reload
        end
      elsif line.start_with? "-setshowtimer:"
        @presentation.show_timer = true
      elsif line.start_with? "-sethidetimer:"
        @presentation.show_timer = false
      elsif line.start_with? "-img,"
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
      elsif line.start_with? "-titlescreen:"
        @presentation.title_screen
        return :fwd
      elsif line.start_with? "-demogame:"
        @presentation.demo_game
        return :fwd
      elsif line.start_with? "-renderbg:"
        MegaMrbc.show_game_bg
      elsif line.start_with? "-scroll:"
        @presentation.scroll_wait
      elsif line.start_with? "-initprogress:"
        @presentation.set_start_page
      elsif line.start_with? "-resettimer:"
        @presentation.set_timer_start
      elsif line.start_with? "-playsound:"
        MegaMrbc.play_se
      elsif @curr_mode == :text
        MegaMrbc.draw_text(line, @x, @y+=1)
      elsif @curr_mode == :code
        render_code_line(line, @x, @y+=1)
      end
    end
    return nil
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
        #MegaMrbc.draw_text(f, x + curr_offset, y)
        #curr_offset += f.length
      else
        MegaMrbc.set_txt_pal(2)
        MegaMrbc.draw_text('"', x + curr_offset, y)
        MegaMrbc.draw_text(f, x + curr_offset + 1, y)
        MegaMrbc.draw_text('"', x + curr_offset + 1 + f.length, y)
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

    MegaMrbc.draw_text(term, x, y)
  end
end

class Presentation
  attr_accessor :show_timer

  def self.start
    self.new.begin_presentation
  end

  def begin_presentation
    main_loop
  end

  def main_loop
    running = true
    cmd = :fwd
    @page_count = MegaMrbc.page_count
    while running do
      MegaMrbc.clear_screen
      if cmd == :fwd
        page = next_page unless @index > @page_count
      elsif cmd == :back
        page = prev_page
      end
      page_cmd = page.render
      wait_vblank(@show_timer)

      # if render returns cmd, use it. Otherwise wait for cmd
      if page_cmd
        cmd = page_cmd
      else
        cmd = wait_cmd
      end
    end
  end

  def next_page
    @index ||= -1
    @index += 1 unless @index == @page_count
    @page = MegaMrbc.read_page_at(@index)
    MegaMrbc.klog("next page, content is #{@page}")
    # MegaMrbc.klog("next, index is #{@index}")
    return Page.new(@page, self)
  end

  def prev_page
    @index ||= 0
    @index -= 1 unless @index < 1
    @page = MegaMrbc.read_page_at(@index)
    # MegaMrbc.klog("prev, index is #{@index}")
    return Page.new(@page, self)
  end

  def set_start_page
    @start_idx = @index
  end

  def wait_cmd
    prev = MegaMrbc.read_joypad
    state = 0
    returning = nil
    while true do
      state = MegaMrbc.read_joypad
      if (state & 0x80 & ~prev) != 0 # start
        returning = :fwd
      elsif (state & 0x40 & ~prev) != 0 # a
        returning = :fwd
      elsif (state & 0x10 & ~prev) != 0 # b
        returning = :back
      elsif (state & 0x20 & ~prev) != 0 # b
        returning = :reload
      end
      prev = state
      wait_vblank(@show_timer)
      break if returning
    end
    return returning
  end

  def set_timer_start
    @start_tick = MegaMrbc.get_current_tick
  end

  def wait_vblank(show_timer = true)
    if show_timer
      MegaMrbc.show_progress(@index, @start_idx.to_i, @page_count - 1) if @index && @page_count
      MegaMrbc.show_timer(@start_tick || 0)
    else
      MegaMrbc.hide_progress
      MegaMrbc.hide_timer
    end
    MegaMrbc.wait_vblank
  end

  def scroll_wait
    prev = MegaMrbc.read_joypad
    while true do
      MegaMrbc.scroll_weird
      pad_state = MegaMrbc.read_joypad
      break if (pad_state & 0x80 & ~prev) != 0 # start
      break if (pad_state & 0x40 & ~prev) != 0 # a
      prev = pad_state
      wait_vblank(false)
    end
  end

  def demo_game
    MegaMrbc.show_game_bg
    pad_state = 0
    v_pos = 0
    v_vel = 0
    prev = MegaMrbc.read_joypad
    spike_pos = 0
    MegaMrbc.show_spikes(spike_pos)
    MegaMrbc.show_runner(v_pos)
    while true do
      MegaMrbc.scroll_game
      spike_pos -= 6
      spike_pos = 0 if spike_pos < -336
      pad_state = MegaMrbc.read_joypad
      break if (pad_state & 0x80 & ~prev) != 0 # start
      if v_pos == 0 && (pad_state & 0x40 & ~prev) != 0 # a
        v_vel = -16
        MegaMrbc.play_jump_se
      else
        v_vel += 2
      end
      v_pos += v_vel
      v_pos = v_vel = 0 if v_pos >= 0
      prev = pad_state
      MegaMrbc.set_spike_pos(spike_pos)
      MegaMrbc.set_runner_pos(v_pos)
      if collided?(v_pos, spike_pos)
        MegaMrbc.sleep_raw(150)
        game_over
        spike_pos = 0
      end
      wait_vblank(false)
    end
    MegaMrbc.hide_runner
    MegaMrbc.hide_spikes
  end

  def game_over
    i = 0
    v_pos = 0
    MegaMrbc.play_death_se
    while i < 30 do
      v_pos += 2
      MegaMrbc.set_runner_pos(v_pos)
      wait_vblank(false)
      i += 1
    end
  end

  def collided?(v_pos, spike_pos)
    v_pos > -4 && spike_pos > -196 && spike_pos < -188
  end

  def title_screen
    state = 0
    count = 0
    prev = MegaMrbc.read_joypad
    MegaMrbc.render_start_bg
    [0, 1, 2].each do |dash_or_plus|
      wait_vblank(false)
      MegaMrbc.render_start_logo(dash_or_plus)
      while true do
        MegaMrbc.scroll_title
        if count / 30 >= 1 # switch every 0.5s
          MegaMrbc.draw_text("Press start", 14, 18)
        else
          MegaMrbc.draw_text("           ", 14, 18)
        end
        state = MegaMrbc.read_joypad
        if (state & 0x80 & ~prev) != 0 # start
          prev = state # do this so loop won't exit immediately after 1st butotn press
          break
        end
        count = 0 if count > 60 # count resets at 60
        count += 1
        prev = state
        wait_vblank(false)
      end
    end
  end
end

Presentation.start
