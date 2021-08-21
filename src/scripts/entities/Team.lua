Team = tdengine.entity('Team')

function Team:init(params)
  self.data = params
  self.active = 1
end

function Team:update(dt)  
end

function Team:get_lead()
  return self.data[1]
end

function Team:make_active(index)
  self.active = index
end
