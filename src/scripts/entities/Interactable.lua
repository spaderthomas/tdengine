local inspect = require('inspect')
local GLFW = require('glfw')

Interactable = tdengine.entity('Interactable')
function Interactable:init(params)
   tdengine.register_interactable(self.id)
   tdengine.disable_collision_detection(self.id)
   self.on_interaction_name = params.on_interaction
end

function Interactable:pre_save()
  local aabb = self:get_component('BoundingBox')
  aabb:should_save(true)
end

function Interactable:save()
   return {
	  
   }
end

function Interactable:update(dt)
   local aabb = self:get_component('BoundingBox')
   aabb:draw()
end

function Interactable:on_interaction()
   local on_interaction = tdengine.on_interactions[self.on_interaction_name]
   on_interaction()
end

tdengine.on_interactions.demo = function()
   tdengine.begin_cutscene('interaction_test')
end
