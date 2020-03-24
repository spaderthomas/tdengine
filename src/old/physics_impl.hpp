void Physics_System::process(float dt) {
	for (auto& mover : movers) {
		def_get_cmp(position, mover.deref(), Position_Component);
		def_get_cmp(movement, mover.deref(), Movement_Component);

		if (movement) {
			position->world_pos += movement->wish;
			movement->wish = glm::vec2(0.f);
		}
	}

	for (auto& element : collisions) {
		auto me = Center_Box::from_entity(element.me);
		auto other = Center_Box::from_entity(element.other);
		if (me && other) {
			glm::vec2 penetration;
			if (are_boxes_colliding(*me, *other, penetration)) {
				std::cout << "Collision found between " << element.me->name << ", " << element.other->name <<  std::endl;
				def_get_cmp(mypos, element.me.deref(), Position_Component);
				mypos->world_pos -= penetration;

				// @hack 9/8/2019: Right now, the game always uses the hero as 'me' and other stuff in the game as 'other'.
				// Clearly, this won't work for real collision detection and is already hacky here!
				if_component(door, element.other, Door_Component) {
					if (element.me->name == HERO_KEY) {
						// @hack 9/20/2019: I mean, you only want this to happen in the game. But this is still hacky as fuck. 
						game.active_level = levels[door->level];
					}
				}
			}
		}
	}
	collisions.clear();
	movers.clear();
}

