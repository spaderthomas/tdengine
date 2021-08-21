OverviewHud = tdengine.entity('OverviewHud')

OverviewHud.Kind = {
  Player = 'Player',
  Opponent = 'Opponent'
}

OverviewHud.State = {
  Idle = 'Idle',
  SlidingIn = 'SlidingIn',
  SlidingOut = 'SlidingOut',
  FinishedSliding = 'FinishedSliding',
}

local arrows = {
  [OverviewHud.Kind.Opponent] = {
	tag = 'opponent_hud_arrow',
	components = {
	  Graphic = {
		layer = 3
	  },
	  Animation = {
		current = 'battle_hud_arrow_right'
	  },
	  Position = {
		world = { x = -.165, y = .8 }
	  },
	  Slide = {
		waypoints = {
		  tdengine.vec2(.157, .8),
		  tdengine.vec2(1.157, .8),
		},
		times = {
		  1,
		  1
		}
	  },
	}
  },
  
  [OverviewHud.Kind.Player] = {
	tag = 'player_hud_arrow',
	components = {
	  Graphic = {
		layer = 3
	  },
	  Animation = {
		current = 'battle_hud_arrow_left'
	  },
	  Position = {
		world = { x = 1.1625, y = .45 }
	  },	params = {
		waypoints = {
		  tdengine.vec2(.843, .45),
		  tdengine.vec2(-.157, .45)
		},
		times = {
		  1,
		  1
		}
	  }
	}
  }
}

local ball_tags = {
  [OverviewHud.Kind.Opponent] = 'opponent_ball_',
  [OverviewHud.Kind.Player] = 'player_ball_',
}

function OverviewHud:init(params)
  self.team = params.team
  self.kind = params.kind
  self.state = OverviewHud.State.Idle

  local arrow = arrows[self.kind]
  tdengine.create_entity('Sprite', arrow)
  self.arrow = tdengine.find_entity_by_tag(arrow.tag):get_component('Slide')

  self.hud_balls = {}
  for i = 0,5 do
	local ball_data = {}
	ball_data.tag = ball_tags[self.kind] .. tostring(i)
	ball_data.params = {
	  index = i,
	  time_in = .2,
	  time_out = .3,
	  team = self.team,
	  kind = BattleHudBall.Kind.Opponent
	}
	
	tdengine.create_entity('BattleHudBall', ball_data)
	self.hud_balls[i] = tdengine.find_entity_by_tag(ball_data.tag)
  end
  self.current_ball = 0
end

function OverviewHud:slide_in()
  for i = 0,5 do
	local ball = self.hud_balls[i]
	local position = ball:get_component('Position')
	position:return_to_original()
  end
  self.hud_balls[0]:slide_in()
  self.current_ball = 0

  self.arrow:next_waypoint()
  
  self.state = OverviewHud.State.SlidingIn
end

function OverviewHud:slide_out()
  self.state = OverviewHud.State.SlidingOut
  
  self.hud_balls[0]:slide_out()
  self.current_ball = 0

  self.arrow:next_waypoint()
end

function OverviewHud:slide_balls(in_or_out)
  local ball = self.hud_balls[self.current_ball]
  if ball.state == BattleHudBall.State.FinishedSliding then
	self.current_ball = self.current_ball + 1
	local next_ball = self.hud_balls[self.current_ball]
	if next_ball then
	  in_or_out(next_ball)
	end
  end
end

function OverviewHud:update(dt)
  if self.state == OverviewHud.State.SlidingIn then
	self:slide_balls(BattleHudBall.slide_in)
  elseif self.state == OverviewHud.State.SlidingOut then
	self:slide_balls(BattleHudBall.slide_out)
  end

  local done = self.arrow.done
  --done = done and self.player_arrow.done
  done = done and self.current_ball == 6
  if done then self.state = OverviewHud.State.FinishedSliding end
end
