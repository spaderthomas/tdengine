Team = tdengine.entity('Team')

function Team:init(params)
  self.data = params
end

function Team:get_lead()
  return self.data[1]
end

function Team:update(dt)
  
end
