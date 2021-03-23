local Teleport = tdengine.action('Teleport')

function Teleport:init(params)
  self.entity = params.entity
  self.marker = params.marker
end

function Teleport:update(dt)
  local entity = tdengine.find_entity(self.entity)
  tdengine.go_to_marker(entity.id, self.marker)
  self.done = true
end

tdengine.actions.Teleport = Teleport
