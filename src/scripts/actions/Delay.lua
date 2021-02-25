local Delay = tdengine.action('Delay')

function Delay:init(params)
  self.time = params.time
end

function Delay:update(dt)
  -- @dev_code
  if tdengine.state['engine:disable_delay_actions'] then
	self.done = true
	return
  end
  
  self.time = self.time - dt
  if self.time <= 0 then
	self.done = true
  end
  end

tdengine.actions.Delay = Delay
