BattleSimulator = tdengine.entity('BattleSimulator')

BattleSimulator.MoveKind = {
  SendLead = 'SendLead'
}

BattleSimulator.ResultKind = {
  MakeActive = 'MakeActive'
}

function BattleSimulator:init(params)
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
