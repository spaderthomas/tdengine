Battle = tdengine.entity('Battle')

local BattleState = {
  MovingPlatforms = 'MovingPlatforms',
  SubmittingIntroText = 'SubmittingIntroText',
  ShowingIntroText = 'ShowingIntroText'
}

-- Trainer platforms
local PlatformType = {
  Player = 'player_platform',
  Opponent = 'opponent_platform'
}

local platforms = {}
platforms[PlatformType.Player] = {
  name = 'Sprite',
  tag = PlatformType.Player,
  components = {
	Position = {
	  world = { x = -.25, y = .355 }
	},
	Animation = {
	  current = 'platform_grass_player'
	}
  }
}
platforms[PlatformType.Opponent] = {
  name = 'Sprite',
  tag = PlatformType.Opponent,
  components = {
	Position = {
	  world = { x = 1.3, y = .65 }
	},
	Animation = {
	  current = 'platform_grass_opponent'
	}
  }
}

local platform_positions = {}
platform_positions[PlatformType.Player] = .3
platform_positions[PlatformType.Opponent] = .65

-- Background
local background = {
  components = {
	Animation = {},
	Position = {
	  world = {
		x = 3,
		y = 3
	  }
	}
  },
  name = "Background",
  params = {}
}

-- Trainer sprites
local opponent_sprite = {
  name = 'Sprite',
  tag = 'opponent',
  components = {
	Graphic = {
	  layer = 2
	},
	Animation = {
	},
	Position = {
	  tag = PlatformType.Opponent,
	  offset = { x = 0, y = .1 }
	}
  }
}

local player_sprite = {
  name = 'Sprite',
  tag = 'player',
  components = {
	Graphic = {
	  layer = 2
	},
	Animation = {
	  current = 'boon_back'
	},
	Position = {
	  tag = PlatformType.Player,
	  offset = { x = .05, y = .07825 }
	}
  }
}

local menu = {
  name = 'BattleMenu',
  params = {},
}

local text_box = {
   name = 'TextBox'
}

function Battle:init(params)
  self.state = BattleState.MovingPlatforms
  self.platform_time = 1
  self.platform_info = {}
  tdengine.follow_player(false)
  tdengine.move_camera(0, 0)

  self.data = tdengine.fetch_module_data('battle/battles/' .. params.battle)
  self.trainer = tdengine.fetch_module_data('battle/trainers/' .. self.data.trainer)
end

function Battle:setup()
  -- Hide the player character
  local player = tdengine.find_entity('Player')
  player:disable_movement()

  local graphic = player:get_component('Graphic')
  graphic:hide()

  -- Load up that background
  background.components.Animation.current = self.data.background
  tdengine.create_entity('Background', background)

  -- Load up them platforms to slide in
  self:load_platform(PlatformType.Player)
  self:load_platform(PlatformType.Opponent)

  -- And the sprites which will be attached to the platforms
  opponent_sprite.components.Animation.current = self.data.trainer
  tdengine.create_entity('Sprite', opponent_sprite)
  
  tdengine.create_entity('Sprite', player_sprite)
  
  -- Make the menu. And then hide it. We have two menus -- one for displaying
  -- text, because thats what the text box is for. And then one for displaying
  -- choices. This one is the one that does choices.
  menu.params = self.data
  tdengine.create_entity('BattleMenu', menu)
  local battle_menu = tdengine.find_entity('BattleMenu')
  local graphic = battle_menu:get_component('Graphic')
  graphic:hide()
  
  tdengine.create_entity('TextBox', {})
end


function Battle:cleanup()
  local player = tdengine.find_entity('Player')
  player:enable_movement()

  local graphic = player:get_component('Graphic')
  graphic:show()
end

function Battle:update(dt)
  if self.state == BattleState.MovingPlatforms then
	local player_done = self:advance_platform(PlatformType.Player, dt)
	local opponent_done = self:advance_platform(PlatformType.Opponent, dt)

	if player_done and opponent_done then
	  self.state = BattleState.ShowingIntroText
	end
  elseif state == BattleState.SubmittingIntroText then
	 local text_box = tdengine.find_entity('TextBox')
	 text_box.begin('fuck my tight rump!')
	 self.state = BattleState.ShowingIntroText
  elseif state == BattleState.ShowingIntroText then
	 local text_box = tdengine.find_entity('TextBox')
	 if text_box.done then
		self.hahahaha = true
	 end
  end
end

function Battle:advance_platform(which, dt)
  local platform = tdengine.find_entity_by_tag(which)
  local position = platform:get_component('Position')
  if double_eq(position.world.x, platform_positions[which], .01) then
	self.platform_info[which].done = true
	return true
  end
  
  local dps = self.platform_info[which].distance / self.platform_time
  local distance = dps * dt
  local id = platform:get_id()

  tdengine.teleport_entity(id, position.world.x + distance, position.world.y)
  
end

function Battle:load_platform(which)
  tdengine.create_entity('Sprite', platforms[which])
  local platform = tdengine.find_entity_by_tag(which)
  local position = platform:get_component('Position')
  local info = {
	done = false,
	distance = platform_positions[which] - position.world.x
  }

  self.platform_info[which] = info
end

