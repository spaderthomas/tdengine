local Delay = tdengine.action('Delay')

function Delay:init(params)
   self.time = params.time
end

function Delay:update(dt)
   self.time = self.time - dt
   if self.time <= 0 then
	  self.done = true
   end
end

tdengine.actions.Delay = Delay
