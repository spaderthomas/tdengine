BattleSimulator = tdengine.entity('BattleSimulator')

BattleSimulator.MoveKind = {
  SendLead = 'SendLead'
}

BattleSimulator.ResultKind = {
  MakeActive = 'MakeActive'
}

function BattleSimulator:init(params)
  self.player = tdengine.find_entity_by_tag('player_team')
  self.opponent = tdengine.find_entity_by_tag('opponent_team')
end

function BattleSimulator:update(dt)
end

function BattleSimulator:submit(action)
  if self.action then
	if self.action.kind == BattleSimulator.MoveKind.SendLead then
	  return {
		{
		  kind = BattleSimulator.ResultKind.MakeActive,
		}
	  }
	end
	
  end
end
