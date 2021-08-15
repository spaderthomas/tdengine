Battle = tdengine.entity('Battle')

local BattleState = {
  MovingPlatforms = 'MovingPlatforms',
  SlidingHudArrow = 'SlidingHudArrow',
  SlidingHudBalls = 'SlidingHudBalls',
  SubmittingIntroText = 'SubmittingIntroText',
  ShowingIntroText = 'ShowingIntroText',
  WaitingForInput = 'WaitingForInput'
}

-- Trainer platforms
local PlatformType = {
  Player = 'player_platform',
  Opponent = 'opponent_platform'
}

local platforms = {}
platforms[PlatformType.Player] = {
  name = 'SlidingSprite',
  tag = PlatformType.Player,
  params = {
	final_position = tdengine.vec2(.3, .355),
	time = 1
  },
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
  params = {
	final_position = tdengine.vec2(.65, .65 ),
	time = 1
  },
  components = {
	Position = {
	  world = { x = 1.3, y = .65 }
	},
	Animation = {
	  current = 'platform_grass_opponent'
	}
  }
}

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

-- HUD stuff
local HudTypes = {
  Player = 'Player',
  Opponent = 'Opponent'
}

local hud = {
  player = {
	arrow = {
	  name = 'SlidingSprite',
	  tag = 'player_hud_arrow',
	  params = {
		final_position = tdengine.vec2(.157, .8),
		time = 1
	  },
	  components = {
		Graphic = {
		  layer = 2
		},
		Animation = {
		  current = 'battle_hud_arrow_right'
		},
		Position = {
		  world = { x = -.165, y = .8 }
		}
	  }
	},
  }
}

--
local menu = {
  name = 'BattleMenu',
  params = {},
}

local text_box = {
  name = 'TextBox'
}

function Battle:init(params)
  self.state = BattleState.Start
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
  tdengine.create_entity('SlidingSprite', platforms[PlatformType.Player])
  self.player_platform = tdengine.find_entity_by_tag(PlatformType.Player)
  tdengine.create_entity('SlidingSprite', platforms[PlatformType.Opponent])
  self.opponent_platform = tdengine.find_entity_by_tag(PlatformType.Opponent)

  -- And the sprites which will be attached to the platforms
  opponent_sprite.components.Animation.current = self.data.trainer
  tdengine.create_entity('Sprite', opponent_sprite)
  tdengine.create_entity('Sprite', player_sprite)

  self:load_hud()
  
  -- Make the menu. And then hide it. We have two menus -- one for displaying
  -- text, because thats what the text box is for. And then one for displaying
  -- choices. This one is the one that does choices.
  menu.params = self.data
  tdengine.create_entity('BattleMenu', menu)
  self.battle_menu = tdengine.find_entity('BattleMenu')
  local graphic = self.battle_menu:get_component('Graphic')
  graphic:hide()
  
  tdengine.create_entity('TextBox', {})
  self.text_box = tdengine.find_entity('TextBox')
  local graphic = self.text_box:get_component('Graphic')
  graphic:show()
end


function Battle:cleanup()
  local player = tdengine.find_entity('Player')
  player:enable_movement()

  local graphic = player:get_component('Graphic')
  graphic:show()
end

function Battle:update(dt)
  if self.state == BattleState.Start then
	-- Kick off the platforms moving
	self.player_platform.start = true
	self.opponent_platform.start = true
	self.state = BattleState.MovingPlatforms
	
  elseif self.state == BattleState.MovingPlatforms then
	-- Nothing to do but wait for the platforms to slide into place
	if self.player_platform.done and self.opponent_platform.done then
	  self.player_arrow.start = true
	  self.state = BattleState.SlidingHudArrow
	end
	
  elseif self.state == BattleState.SlidingHudArrow then
	-- Slide the arrow...
	if self.player_arrow.done then
	  self.hud_balls[0].start = true
	  self.state = BattleState.SlidingHudBalls
	end
	
  elseif self.state == BattleState.SlidingHudBalls then
	self.state = BattleState.SubmittingIntroText
	
  elseif self.state == BattleState.SubmittingIntroText then
	self.text_box:begin('someone wants to battle...')
	self.state = BattleState.ShowingIntroText
	
  elseif self.state == BattleState.ShowingIntroText then
	local text_box = tdengine.find_entity('TextBox')
	if text_box.done then
	  self.state = BattleState.WaitingForInput
	end
	
  end
end

function Battle:load_hud()
  tdengine.create_entity('SlidingSprite', hud.player.arrow)
  self.player_arrow = tdengine.find_entity_by_tag('player_hud_arrow')

  self.hud_balls = {}
  for i = 0,5 do
	local ball_data = {}
	ball_data.tag = 'player_ball_' .. tostring(i)
	ball_data.params = {
	  index = i,
	  time = .5
	}
	
	tdengine.create_entity('BattleHudBall', ball_data)
	self.hud_balls[i] = tdengine.find_entity_by_tag(ball_data.tag)
  end
end
