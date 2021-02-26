local Compound = tdengine.action('Compound')

function Compound:init(params)
  self.actions = {}
  for index, data in pairs(params.actions) do
	local action = tdengine.create_action(data.name, data)
	self.actions[index] = action
  end
end

function Compound:update(dt)
  local done = tdengine.update_actions(dt, self.actions)
  self.done = done
end

tdengine.actions.Compound = Compound
