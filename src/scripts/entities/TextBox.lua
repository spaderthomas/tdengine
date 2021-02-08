TextBox = tdengine.entity('TextBox')
function TextBox:init()
   tdengine.register_collider(self:get_id())
   self:reset()
end

function TextBox:reset()
   self.text = nil
   self.lines = {}
   self.waiting = false
   self.active = false
   self.done = false
   self.time_accumulated = 0
   self.time_per_update = 4 * tdengine.frame_time
   self.line_point = 1
   self.point = 0
   self.max_point = 0
end

function TextBox:begin(text)
   self:reset()
   self.text = text
   self.active = true
   
   -- Calculate the length of the text area, in pixels, for wrapping purposes
   local text_area = tdengine.sprite_size('text_box.png').x
   local padding = .075
   local screen = tdengine.screen_dimensions()
   text_area = screen.x * (text_area - padding)

   -- Split the text into lines
   local line = ''
   local line_size = 0
   local word = ''
   local word_size = 0
   for i = 1, #self.text do
	  self.max_point = self.max_point + 1
	  
	  local c = self.text:sub(i,i)
	  
	  -- On a newline, you're done with whatever line you were working on.
	  if is_newline(c) then
		 line = line .. word
		 table.insert(self.lines, line)

		 line = ''
		 word = ''
	  -- Space is tricky: If the last word was too long for the line, then
	  -- we need to move to the next line and take the last word with us
	  elseif is_space(c) then
		 -- If the word fits, just add it to the line
		 if line_size + word_size <= text_area then
			line = line .. word .. ' '
			line_size = line_size + word_size + (tdengine.font.advance(' ') / 64)
		 -- If it does not, add it to the next line
		 else
			table.insert(self.lines, line)
			line = word .. ' '
			line_size = word_size + (tdengine.font.advance(' ') / 64)
		 end

		 word = ''
		 word_size = 0
	  -- Anything else, just keep parsing this word
	  else
		 word_size = word_size + (tdengine.font.advance(c) / 64)
		 word = word .. c
	  end
   end

   line = line .. word
   table.insert(self.lines, line)

   print(inspect(self.lines))
end

function TextBox:update(dt)
   self.time_accumulated = self.time_accumulated + dt
   if not self.waiting and self.time_accumulated >= self.time_per_update then
	  self.time_accumulated = 0
   end

   -- get text position from combination of: your position, padding
   -- text rendering position rn uses bottom left
   local position = tdengine.vec2(self:get_component('Position').world)
   --tdengine.teleport_entity(self:get_id(), position.x, .125)
   
   local text_area = tdengine.vec2(tdengine.sprite_size('text_box.png'))
   local extents = text_area:scale(.5)
   local padding = tdengine.vec2(.02, 0.04)
   local text_start = tdengine.vec2(
	  position.x - extents.x + padding.x,
	  position.y + extents.y - padding.y)

   for index, line in pairs(self.lines) do
	  tdengine.draw_text(line, text_start.x, text_start.y, 0)
	  text_start.y = text_start.y + .05
   end
end

function TextBox:skip()
   print('skipping to end...')
end
