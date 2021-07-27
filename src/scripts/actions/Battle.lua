local Battle = tdengine.action('Battle')

function Battle:init(params)
  local battle = ternary(params.battle, params.battle, '000_missing')
  local battle_data = tdengine.fetch_module_data('battles/' .. battle)
  print(inspect(battle_data))
end

function Battle:update(dt)
  self.done = true
end

tdengine.actions.Battle = Battle
