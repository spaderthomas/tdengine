Debug = tdengine.component('Debug')

function Debug:init(params)
  self.last_message = ''
end

function Debug:update()
end

function Debug:print_once(message)
  if message ~= self.last_message then
	print(message)
  end
  self.last_message = message
end
