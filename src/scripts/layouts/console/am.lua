local player = tdengine.find_entity('Player')
local animation = player:get_component('Animation')
animation:begin('red_stand')
