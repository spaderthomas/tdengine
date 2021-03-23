local CreateEntity = tdengine.action('CreateEntity')

function CreateEntity:init(params)
  self.entity = params.entity
  self.data = params.data
end

function CreateEntity:update(dt)
  tdengine.create_entity(self.entity, self.data)
  self.done = true
end

tdengine.actions.CreateEntity = CreateEntity
