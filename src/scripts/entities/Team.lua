Team = tdengine.entity('Team')

function Team:init(params)
  self.souls = {}
  for index, soul_data in pairs(params) do
	soul_data.fainted = false
	local entity = {
	  params = soul_data
	}
	local id = tdengine.create_entity('Soul', entity)
	local soul = tdengine.find_entity_by_id(id)
	table.insert(self.souls, soul)
  end
  self.active = 1
end

function Team:update(dt)  
end

function Team:get_lead()
  return self.souls[1]
end

function Team:get_active()
  return self.souls[self.active]
end

function Team:get_switchable_souls()
  local souls = {}
  for index, soul in pairs(self.souls) do
	if not soul.fainted and index ~= self.active then
	  table.insert(souls, soul)
	end
  end
  
  return souls
end

function Team:make_active(index)
  self.active = index
end
