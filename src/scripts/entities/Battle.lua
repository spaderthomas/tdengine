Battle = tdengine.entity('Battle')

local GLFW = require('glfw')

local BattleState = {
  MovingPlatforms = 'MovingPlatforms',
  SlidingHud = 'SlidingHud',
  ShowingIntroText = 'ShowingIntroText',
  WaitingForLeadText = 'WaitingForLeadText',
  WaitingToSlideOutHud = 'WaitingToSlideOutHud',
  SlidingOutHud = 'SlidingOutHud',
  SlidingOutOpponentTrainer = 'SlidingOutOpponentTrainer',
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

  -- Platforms
  tdengine.create_entity('BattlePlatforms', {})
  self.platforms = tdengine.find_entity('BattlePlatforms')

  -- Simulator
  tdengine.create_entity('BattleSimulator', {})
  self.simulator = tdengine.find_entity('BattleSimulator')
  
  -- Make the menu. And then hide it. We have two menus -- one for displaying
  -- text, because thats what the text box is for. And then one for displaying
  -- choices. This one is the one that does choices.
  tdengine.create_entity('BattleMenu', {})
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
	self.platforms:next_waypoint()
	self.state = BattleState.MovingPlatforms
	
  elseif self.state == BattleState.MovingPlatforms then
	-- Nothing to do but wait for the platforms to slide into place
	if self.platforms:done() then
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
	  self.state = BattleState.WaitingToSlideOutHud
	  self.time_until_lead_animation = dt
	end
	
  elseif self.state == BattleState.WaitingToSlideOutHud then
	self.time_until_lead_animation = self.time_until_lead_animation - dt
	if double_eq(self.time_until_lead_animation, 0, .0001) then
	  self.overview_huds.opponent.entity:slide_out()
	  self.state = BattleState.SlidingOutHud
	end

  elseif self.state == BattleState.SlidingOutHud then
	if self.overview_huds.opponent.entity.state == OverviewHud.State.FinishedSliding then
	  self.platforms:slide_out_opponent_sprite()
	  self.state = BattleState.SlidingOutOpponentTrainer
	end

  elseif self.state == BattleState.SlidingOutOpponentTrainer then
	if self.platforms:done() then
	  local lead = self.opponent_team:get_lead()
	  
	end
	
  else
	self.opponent_team:make_active(1)
	
  end
end
