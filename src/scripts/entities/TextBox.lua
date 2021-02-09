TextBox = tdengine.entity('TextBox')
function TextBox:init()
   tdengine.register_collider(self:get_id())
   self:reset()
end

function TextBox:reset()
   self.text = nil
   self.waiting = false
   self.active = false
   self.done = false
   self.time_accumulated = 0
   self.time_per_update = 4 * tdengine.frame_time
   self.point = 0
   self.max_point = 0

   -- If there is a line break at character i, that means that you want the
   -- last character of this line to be text[i]
   self.line_breaks = {}
end

function TextBox:print_lines()
   print('print_lines(): breaks are ' .. inspect(self.line_breaks))

   print('print_lines(): lines begin')
   local line_start = 1
   for index, line_break in pairs(self.line_breaks) do
	  local message = '(' .. tostring(line_start) .. ', ' .. tostring(line_break) .. '): '
	  message = message .. '"' .. self.text:sub(line_start, line_break ) .. '"'
	  print(message)
	  line_start = line_break + 1
   end
   print('print_lines(): lines end')

end

function TextBox:begin(text)
   self:reset()
   self.text = text
   self.active = true
   self.max_point = text:len()
   
   -- Calculate the length of the text area, in pixels, for wrapping purposes
   local content_width = tdengine.sprite_size('text_box.png').x
   local padding = .075
   local screen = tdengine.screen_dimensions()
   content_width = screen.x * (content_width - padding)

   -- Split the text into lines
   local line_size = 0
   local word_size = 0

   local last_word_point = 1
   for point = 1, #self.text do
	  local c = self.text:sub(point, point)
	  if is_newline(c) then
		 -- On a newline, you're done with whatever line you were working on.
		 table.insert(self.line_breaks, point)
		 line_size = 0
		 word_size = 0
	  elseif is_space(c) then
		 -- Space is tricky: If the last word was too long for the line, then
		 -- we need to move to the next line and take the last word with us
		 if line_size + word_size <= content_width then
			-- If the word fits, just add it to the line
			line_size = line_size + word_size + (tdengine.font.advance(' ') / 64)
			last_word_point = point
		 else
			-- The word would make us spill over. Line break before this word, and
			-- add this word's length to the next line
			table.insert(self.line_breaks, last_word_point)
			line_size = word_size + (tdengine.font.advance(' ') / 64)
		 end

		 word_size = 0
	  else
		 -- Anything else, just keep parsing this word
		 word_size = word_size + (tdengine.font.advance(c) / 64)
	  end
   end
   
   if line_size + word_size <= content_width then
	  table.insert(self.line_breaks, #self.text)
   else
	  table.insert(self.line_breaks, last_word_point)
	  table.insert(self.line_breaks, #self.text)
   end
   
   table.sort(ordered_line_breaks)
end

function TextBox:update(dt)
   self.time_accumulated = self.time_accumulated + dt
   if not self.waiting and self.time_accumulated >= self.time_per_update then
	  self.time_accumulated = 0
   end

   self.point = math.min(self.point + 1, self.max_point)

   local position = self:get_component('Position').world
   
   local text_area = tdengine.vec2(tdengine.sprite_size('text_box.png'))
   local extents = text_area:scale(.5)
   local padding = tdengine.vec2(.02, 0.04)
   local text_start = tdengine.vec2(
	  position.x - extents.x + padding.x,
	  position.y + extents.y - padding.y)

   local line_start = 1
   local remaining = self.point
   for index, line_break in pairs(self.line_breaks) do
	  if not (remaining > 0) then break end
	  
	  local len = line_break - line_start
	  local line = nil
	  if len <= remaining then
		 line = self.text:sub(line_start, line_break)
		 remaining = remaining - len
	  else
		 line = self.text:sub(line_start, line_start + remaining)
		 remaining = 0
	  end

	  tdengine.draw_text(line, text_start.x, text_start.y, 0)
	  text_start.y = text_start.y - .02
	  line_start = line_break + 1
   end

   tdengine.do_once(function() self:print_lines() end)
end

function TextBox:skip()
   print('skipping to end...')
end
