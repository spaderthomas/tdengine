Trigger = tdengine.entity('Trigger')

function Trigger:init(params)
  self.on_collision_proc = params.on_collision
  tdengine.register_trigger(self.id)
end

function Trigger:update(dt)
end

function Trigger:pre_save()
  local aabb = self:get_component('BoundingBox')
  aabb:should_save(true)
end

function Trigger:save()
  return {
	on_collision = self.on_collision_proc
  }
end

function Trigger:on_collision(other)
  if other:get_name() == 'Player' then
	local proc = tdengine.triggers[self.on_collision_proc]
	if proc then proc(self) end
  end
end

tdengine.triggers = {
  ['demo:trigger_battle'] = function(self)
	tdengine.do_cutscene_from_name('introduce_hub')
	tdengine.destroy_entity(self.id)
  end,
  ['main:introduce_hub'] = function(self)
	tdengine.do_cutscene_from_name('introduce_hub')
	tdengine.destroy_entity(self.id)
  end,
  ['main:introduce_librarian'] = function(self)
	tdengine.do_cutscene_from_name('librarian_intro')
	tdengine.destroy_entity(self.id)
  end
}
