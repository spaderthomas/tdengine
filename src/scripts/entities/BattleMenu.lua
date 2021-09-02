BattleMenu = tdengine.entity('BattleMenu')
function BattleMenu:init(params)
  self.team = tdengine.find_entity_by_tag('player_team')
end

function BattleMenu:update(dt)
  
end

function BattleMenu:enable(dt)
  local active = self.team:get_active()
  self.choices = {}

  self.moves = active:moves()
  for index, move in pairs(self.moves) do
	table.insert(self.choices, move:flavor_name())
  end

  self.switchable = self.team:switchable_souls()
  for index, soul in pairs(self.switchable) do
	local template = "Switch to %s!"
	local switch_message = string.format(template, soul:flavor_name())
	table.insert(self.choices, switch_message)
  end

  local text_box = tdengine.find_entity('TextBox')
  text_box:choose(self.choices)
end
