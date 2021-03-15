local player = tdengine.find_entity('Player')
tdengine.teleport_entity(player.id, .2, .2)
tdengine.snap_to_player()
