BattleMenu = tdengine.entity('BattleMenu')
function BattleMenu:init(params)
  self.team = tdengine.find_entity_by_tag('player_team')
  self.enabled = true
end

function BattleMenu:update(dt)

end

function BattleMenu:enable(dt)
  self.enabled = true
  
  local active = self.team:get_active()
  self.moves = active:get_moves()
  self.switches = self.team:get_switchable_souls()
  
  local graphic = self:get_component('Graphic')
  graphic:show()
end

