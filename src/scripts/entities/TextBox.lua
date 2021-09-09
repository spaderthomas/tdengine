TextBox = tdengine.entity('TextBox')
function TextBox:init(params)
  self:reset()

  local graphic = self:get_component("Graphic")
  graphic:hide()
end

function TextBox:reset()
  self.text = nil
  self.active = false -- Was some text submitted to us?
  self.done = false -- Is all the requested text being displayed?
  self.time_accumulated = 0
  self.time_per_update = tdengine.frame_time
  self.point = 0
  self.max_point = 0
  self.choosing = false
  self.highlighted_choice = 0

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

function line_breaks(text)
  local breaks = {}

  -- Calculate the length of the text area, in pixels, for wrapping purposes
  local content_width = tdengine.sprite_size('text_box').x
  local padding = .075
  local screen = tdengine.screen_dimensions()
  content_width = screen.x * (content_width - padding)

  -- Split the text into lines
  local line_size = 0
  local word_size = 0

  local last_word_point = 1
  for point = 1, #text do
	local c = text:sub(point, point)
	local advance = tdengine.font_advance(tdengine.options.dialogue_font, c)
	advance = advance / 64
	if is_newline(c) then
	  -- On a newline, you're done with whatever line you were working on.
	  table.insert(breaks, point)
	  line_size = 0
	  word_size = 0
	elseif is_space(c) then
	  -- Space is tricky: If the last word was too long for the line, then
	  -- we need to move to the next line and take the last word with us
	  if line_size + word_size <= content_width then
		-- If the word fits, just add it to the line
		
		line_size = line_size + word_size + advance
		last_word_point = point
	  else
		-- The word would make us spill over. Line break before this word, and
		-- add this word's length to the next line
		table.insert(breaks, last_word_point)
		line_size = word_size + advance
	  end

	  word_size = 0
	else
	  -- Anything else, just keep parsing this word
	  word_size = word_size + advance
	end
  end

  if line_size + word_size <= content_width then
	table.insert(breaks, #text)
  else
	table.insert(breaks, last_word_point)
	table.insert(breaks, #text)
  end
  
  table.sort(breaks)
  return breaks
end

function TextBox:choose(choices)
  self:reset()
  self.text = {}
  self.active = true
  self.choosing = true
  self.done = true -- Choices all display immediately
  self.highlighted_choice = 1

  local graphic = self:get_component("Graphic")
  graphic:show()

  for index, choice in pairs(choices) do
	self.line_breaks[index] = line_breaks(choice)
	table.insert(self.text, choice)
  end
end

function TextBox:line_size()
  local info = tdengine.font_info(tdengine.options.dialogue_font)
  local screen = tdengine.screen_dimensions()
  return (info.largest.y / screen.y)
end

function TextBox:begin(text)
  self:reset()
  self.text = text
  self.active = true
  self.max_point = text:len()
  
  local graphic = self:get_component("Graphic")
  graphic:show()
  
  self.line_breaks = line_breaks(text)
end

function TextBox:update(dt)
  self.time_accumulated = self.time_accumulated + dt
  if self.time_accumulated >= self.time_per_update then
	self.point = math.min(self.point + 1, self.max_point)
	self.done = self.point == self.max_point
	self.time_accumulated = 0
  end

  local position = self:get_component('Position').world
  
  local text_area = tdengine.vec2(tdengine.sprite_size('text_box'))
  local extents = text_area:scale(.5)
  local padding = tdengine.vec2(.03, 0.075 + (self:line_size() / 4))
  local text_start = tdengine.vec2(
	position.x - extents.x + padding.x,
	position.y + extents.y - padding.y)

  if self.choosing then
	for choice_index, choice in pairs(self.text) do
	  local flags = ternary(choice_index == self.highlighted_choice, tdengine.text_flags.highlighted, 0)

	  local line_breaks = self.line_breaks[choice_index]
	  local line_start = 1
	  for index, line_break in pairs(line_breaks) do
		local line = choice:sub(line_start, line_break)

		tdengine.draw.text(line, text_start.x, text_start.y, flags)
		if index ~= #line_breaks then
		  text_start.y = text_start.y - self:line_size()
		  line_start = line_break + 1
		end
	  end

	  text_start.y = text_start.y - self:line_size()
	end
  else
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

	  tdengine.draw.text(line, text_start.x, text_start.y, 0)

	  -- Bump down to the next line
	  text_start.y = text_start.y - self:line_size()
	  line_start = line_break + 1
	end
  end
end

function TextBox:skip()
  self.point = self.max_point
end

function TextBox:highlight_choice(index)
  self.highlighted_choice = index
end
