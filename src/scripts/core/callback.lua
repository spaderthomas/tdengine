function update_entities(dt)
  for id, entity in pairs(tdengine.entities) do
	entity:update(dt)
  end
end

function update_components(dt)
  for id, component in pairs(tdengine.components) do
	component:update(dt)
  end
end

function update_entity(id, dt)
  local entity = tdengine.entities[id]
  if entity then
	entity:update(dt)
	return
  end
end

function update_component(id, dt)
  local component = tdengine.components[id]
  component:update(dt)
end

function on_collision(id, other)
  local entity = tdengine.entities[id]
  local other = tdengine.entities[other]

  if not entity then
	print('@bad_entity_on_collision: ' .. inspect(entity) .. ', ' .. inspect(other))
	return
  end

  if not other then
	print('@bad_other_on_collision: ' .. inspect(other) .. ', ' .. inspect(entity))
	return
  end
  
  if entity.on_collision then
	entity:on_collision(other)
  end
end

function on_interaction(id)
  local entity = tdengine.entities[id]

  if not entity then
	print('@no_entity_on_interaction, ' .. tostring(id))
  end
  
  if entity.on_interaction then
	entity:on_interaction()
  end
end
