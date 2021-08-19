Battle = tdengine.entity('Battle')

local GLFW = require('glfw')

local BattleState = {
  MovingPlatforms = 'MovingPlatforms',
  SlidingHud = 'SlidingHud',
  ShowingIntroText = 'ShowingIntroText',
  WaitingForLeadText = 'WaitingForLeadText',
  WaitingToPlayLeadAnimation = 'WaitingToPlayLeadAnimation',
  PlayingLeadAnimation = 'PlayingLeadAnimation',
  SendingOpponentLead = 'SendingOpponentLead',
  WaitingForInput = 'WaitingForInput'
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
	},
	Slide = {
	  waypoints = { tdengine.vec2(.25, .355) },
	  times = { 1 }
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
	},
	Slide = {
	  waypoints = { tdengine.vec2(.65, .65 ) },
	  times = { 1 }
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

  -- Create entities that store the team state
  local opponent_team_data = {
	tag = 'opponent_team',
	params = self.data.team,
	components = {}
  }
  tdengine.create_entity('Team', opponent_team_data)
  self.opponent_team = tdengine.find_entity_by_tag(opponent_team_data.tag)

  self.overview_huds = {
	opponent = {
	  entity = nil,
	  data = {
		tag = 'opponent_overview_hud',
		params = {
		  team = self.opponent_team,
		  kind = OverviewHud.Kind.Opponent
		}
	  }
	}
  }
  tdengine.create_entity('OverviewHud', self.overview_huds.opponent.data)
  local tag = self.overview_huds.opponent.data.tag
  self.overview_huds.opponent.entity = tdengine.find_entity_by_tag(tag)
  
  -- Load up that background
  background.components.Animation.current = self.data.background
  tdengine.create_entity('Background', background)
  
  -- Load up them platforms to slide in
  tdengine.create_entity('Sprite', platforms[PlatformType.Player])
  self.player_platform = tdengine.find_entity_by_tag(PlatformType.Player)
  tdengine.create_entity('Sprite', platforms[PlatformType.Opponent])
  self.opponent_platform = tdengine.find_entity_by_tag(PlatformType.Opponent)

  
  -- And the sprites which will be attached to the platforms
  opponent_sprite.components.Animation.current = self.data.trainer
  tdengine.create_entity('Sprite', opponent_sprite)
  tdengine.create_entity('Sprite', player_sprite)


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

function Battle:build_lead_message()
  local lead_name = self.opponent_team:get_lead().name
  local lead = tdengine.fetch_module_data('battle/souls/' .. lead_name)
  local format = '%s sent out %s!'
  return string.format(format, self.trainer.flavor_name, lead.flavor_name)
end

function Battle:update(dt)
  if self.state == BattleState.Start then
	-- Kick off the platforms moving
    --self.player_platform:get_component('Slide'):next_waypoint()
	--self.opponent_platform:get_component('Slide'):next_waypoint()
	self.state = BattleState.MovingPlatforms
	
  elseif self.state == BattleState.MovingPlatforms then
	-- Nothing to do but wait for the platforms to slide into place
	if self.player_platform.done and self.opponent_platform.done then
	  self.overview_huds.opponent.entity:slide_in()
	  self.text_box:begin(self.data.intro_text)
	  self.state = BattleState.SlidingHud
	end
	
  elseif self.state == BattleState.SlidingHud then
	if self.overview_huds.opponent.entity.state == OverviewHud.State.FinishedSliding then
	  self.state = BattleState.ShowingIntroText
	end
  elseif self.state == BattleState.ShowingIntroText then
	local input = self:get_component('Input')
	if input:was_pressed(GLFW.Keys.SPACE) then
	  self.text_box:begin(self:build_lead_message())
	  self.state = BattleState.WaitingForLeadText
	end
  elseif self.state == BattleState.WaitingForLeadText then
	if self.text_box.done then
	  self.state = BattleState.WaitingToPlayLeadAnimation
	  self.time_until_lead_animation = 1
	end
  elseif self.state == BattleState.WaitingToPlayLeadAnimation then
	self.time_until_lead_animation = self.time_until_lead_animation - dt
	if double_eq(self.time_until_lead_animation, 0, .0001) then
	  self.overview_huds.opponent.entity:slide_out()
	  self.state = BattleState.PlayingLeadAnimation
	end
  end
end
