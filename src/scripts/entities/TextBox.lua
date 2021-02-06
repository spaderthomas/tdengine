TextBox = tdengine.entity('TextBox')
function TextBox:init()
   tdengine.register_collider(self:get_id())

   self.text = nil
   self.lines = {}
   self.waiting = false
   self.active = false
   self.time_accumulated = 0
   self.time_per_update = 4 * tdengine.frame_time
end

function TextBox:begin(text)
   self.text = text
   self.lines = {}
   self.waiting = false
   self.active = true

   local text_area = tdengine.sprite_size('text_box.png').x
   text_area = text_area * 320

   local line = ''
   local line_size = 0
   local word = ''
   local word_size = 0
   for i = 1, #self.text do
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

   for index, line in pairs(self.lines) do
	  print(line)
   end
end

function TextBox:update(dt)
end
