HubManager = tdengine.entity('HubManager')
function HubManager:init(params)
end

function HubManager:setup()
  local story_marker = tdengine.state['main:story_marker']
  self.last_story_marker = story_marker
  self.destroy = {}

  -- Create entities
  local entities = HubManager.entities[story_marker]
  if entities then
	for index, entity in pairs(entities) do
	  local id = tdengine.create_entity(entity.name, entity)
	  table.insert(self.destroy, id)
	end
  end

  -- Run whatever bullshit custom setup code you want
  if story_marker == tdengine.story_markers.LEFT_JANITOR_CLOSET then
	print('custom setup code')
  end
end

function HubManager:cleanup()
  for index, id in pairs(self.destroy) do
	tdengine.destroy_entity(id)
  end
end

function HubManager:update(dt)
end


HubManager.entities = {}
HubManager.entities[tdengine.story_markers.LEFT_JANITOR_CLOSET] = {
  {
	name = 'Trigger',
	params = { on_collision = 'main:introduce_hub' },
	components = {
	  BoundingBox = {
		extents = { x = .029, y = .235 }
	  },
	  Position = {
		world = { x = 2.22, y = 2.37 }
	  }
	}
  },
}
