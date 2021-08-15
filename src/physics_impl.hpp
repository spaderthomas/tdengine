bool are_boxes_colliding(Center_Box a, Center_Box b, glm::vec2& penetration) {
	// First, calculate the Minkowski difference
	Center_Box minkowski;
	minkowski.extents = a.extents + b.extents;
	float a_left = a.origin.x - .5f * a.extents.x;
	float b_right = b.origin.x + .5f * b.extents.x;
	minkowski.origin.x = a_left - b_right + .5f * minkowski.extents.x;
	float a_top = a.origin.y + .5f * a.extents.y;
	float b_bottom = b.origin.y - .5f * b.extents.y;
	minkowski.origin.y = a_top - b_bottom - .5f * minkowski.extents.y;

	// If the Minkowski difference intersects the origin, there's a collision
	auto verts = minkowski.as_points();
	if (verts.right >= 0 && verts.left <= 0 && verts.top >= 0 && verts.bottom <= 0) {
		// The pen vector is the shortest vector from the origin of the MD to an edge.
		// You know this has to be a vertical or horizontal line from the origin (these are by def. the shortest)
		float min = 100000.f;
		if (abs(verts.left) < min) {
			min = abs(verts.left);
			penetration = glm::vec2(verts.left, 0.f);
		}
		if (abs(verts.right) < min) {
			min = abs(verts.right);
			penetration = glm::vec2(verts.right, 0.f);
		}
		if (abs(verts.top) < min) {
			min = abs(verts.top);
			penetration = glm::vec2(0.f, verts.top);
		}
		if (abs(verts.bottom) < min) {
			min = abs(verts.bottom);
			penetration = glm::vec2(0.f, verts.bottom);
		}

		return true;
	}

	penetration = glm::vec2(0.f);
	return false;
}

bool are_boxes_colliding(Points_Box a, Points_Box b, glm::vec2& penetration) {
	//@hack just write this
	return are_boxes_colliding(a.as_center_box(), b.as_center_box(), penetration);
}

Points_Box Center_Box::as_points() {
	float left = origin.x - extents.x / 2;
	float right = origin.x + extents.x / 2;
	float top = origin.y + extents.y / 2;
	float bottom = origin.y - extents.y / 2;

	return { top, bottom, left, right };
}
Center_Box Center_Box::from_points(Points_Box& points) {
	Center_Box box;
	box.origin = glm::vec2((points.left + points.right) / 2, (points.top + points.bottom) / 2);
	box.extents = glm::vec2(points.right - points.left, points.top - points.bottom);
	return box;
}

Center_Box box_from_collider(Position* position, Collider* collider) {
	Center_Box box;
	box.origin = *position + collider->offset;
	box.extents = collider->extents;
	return box;
}

Center_Box Center_Box::from_entity(int entity) {
	auto box = Center_Box();
	
	auto& physics = get_physics_engine();
	auto position = physics.get_position(entity);
	auto collider = physics.get_collider(entity);
	if (!position || !collider) return box; // @log
	
	box.origin = *position + collider->offset;
	box.extents = collider->extents;
	return box;
}

void Points_Box::convert_screen_to_gl() {
	left = gl_from_screen(left);
	right = gl_from_screen(right);
	top = gl_from_screen(top);
	bottom = gl_from_screen(bottom);
}
Center_Box Points_Box::as_center_box() {
	Center_Box center_box;
	center_box.origin = {
		(left + right) / 2.f,
		(top + bottom) / 2.f
	};
	center_box.extents = {
		(right - left),
		(top - bottom)
	};

	return center_box;
}

bool point_inside_box(glm::vec2& screen_pos, Center_Box& box) {
	Points_Box points = box.as_points();
	if (screen_pos.x > points.left && screen_pos.x < points.right
		&& screen_pos.y > points.bottom && screen_pos.y < points.top) {
		return true;
	}

	return false;
}

bool point_inside_entity(float x, float y, int entity) {
	auto& physics_engine = get_physics_engine();
	auto position = physics_engine.get_position(entity);
	auto collider = physics_engine.get_collider(entity);
	if (!position || !collider) return false;

	
	float xl = position->x + collider->offset.x + (collider->extents.x / 2);
	float xs = position->x + collider->offset.x - (collider->extents.x / 2);
	bool inside_x = (xs < x) && (x < xl);

	float yl = position->y + collider->offset.y + (collider->extents.y / 2);
	float ys = position->y + collider->offset.y - (collider->extents.y / 2);
	bool inside_y = (ys < y) && (y < yl);

	return inside_x && inside_y;
}


void PhysicsEngine::update(float dt) {
	collisions.clear();

	// Keep a list of stuff we move, so we know what needs updating in Lua
	std::vector<int> moved_entities;
	
	for (auto& request : requests) {
		moved_entities.push_back(request.entity);

		auto mover_position = get_position(request.entity);
		if (request.flags & MoveFlags::AbsolutePosition) {
			*mover_position = request.wish;
		} else {
			*mover_position += request.wish;
		}

		if (request.flags & MoveFlags::BypassCollision) {
			continue;
		}

		auto mover_box = Center_Box::from_entity(request.entity);

		// Handle regular collisions
		for (auto& [id, other] : collidable) {
			if (id == request.entity) continue;

			auto other_box = Center_Box::from_entity(id);
			
			glm::vec2 penetration;
			if (are_boxes_colliding(mover_box, other_box, penetration)) {
				*mover_position -= penetration;

				CollisionInfo info;
				info.entity = request.entity;
				info.other = id;
				collisions.push_back(info);
			}
		}

		// Handle collisions with triggers
		for (auto& [id, other] : triggers) {
			Center_Box other_box;
	
			auto position = get_position(id);
			auto collider = get_trigger(id);
			if (!position || !collider) continue;
			
			other_box.origin = *position + collider->offset;
			other_box.extents = collider->extents;

			glm::vec2 penetration;
			if (are_boxes_colliding(mover_box, other_box, penetration)) {
				CollisionInfo info;
				info.entity = request.entity;
				info.other = id;
				collisions.push_back(info);
			}
		}
	}

	requests.clear();

	// Now that all first-level movements have been resolved, let attached entities try to warp to
	// what they're attached to. Gets picked up next resolve cycle.
	for (auto& info : attached_entities) {
		auto position = get_position(info.entity);
		auto desired = get_position(info.attached_to);

		// If you don't do this, you'll have an infinite resolve as each resolve cycle submits
		// another movement to resolve
		if (vec_almost_equals(*position - info.offset, *desired)) continue;

		MoveRequest request;
		request.entity = info.entity;
		request.wish = *desired + info.offset;
		request.flags = MoveFlags::AbsolutePosition | MoveFlags::BypassCollision;
		
		requests.push_back(request);
	}

	// Push back what you just changed to Lua
	for (auto entity : moved_entities) {
		auto position = get_position(entity);
		if (!position) continue;

		auto component = Lua.get_component(entity, "Position");
		component["world"]["x"] = position->x;
		component["world"]["y"] = position->y;
	}

}

void PhysicsEngine::add_collider(int entity, Collider collider) {
	collidable[entity] = collider;
}

bool PhysicsEngine::has_collider(int entity) {
	return collidable.find(entity) != collidable.end();
}

Collider* PhysicsEngine::get_collider(int entity) {
	auto it = collidable.find(entity);
	if (it == collidable.end()) {
		return nullptr;
	}

	return &(it->second);
}

Collider* PhysicsEngine::get_trigger(int entity) {
	auto it = triggers.find(entity);
	if (it == triggers.end()) {
		return nullptr;
	}

	return &(it->second);
}

void PhysicsEngine::add_trigger(int entity, Collider collider) {
	triggers[entity] = collider;
}

void PhysicsEngine::add_raycast(int entity, Collider collider) {
	raycast[entity] = collider;
}

Collider* PhysicsEngine::get_raycast(int entity) {
	auto it = raycast.find(entity);
	if (it == raycast.end()) {
		return nullptr;
	}

	return &(it->second);
}

int PhysicsEngine::ray_cast(float x, float y) {
	glm::vec2 cursor(x, y);

	auto check_single_entity = [&](auto id, auto collider) {
		auto position = get_position(id);
		auto box = box_from_collider(position, collider);
		if (point_inside_box(cursor, box)) {
			return true;
		}
		
		return false;
	};
		
	for (auto& [id, collider] : collidable) {
		if (check_single_entity(id, &collider)) return id;
	}

	for (auto& [id, collider] : triggers) {
		if (check_single_entity(id, &collider)) return id;
	}
	
	for (auto& [id, collider] : raycast) {
		if (check_single_entity(id, &collider)) return id;
	}

	return -1;
}

void PhysicsEngine::add_position(int entity, Position position) {
	positions[entity] = position;
}

Position* PhysicsEngine::get_position(int entity) {
	auto it = positions.find(entity);
	if (it == positions.end()) {
		tdns_log.write("@no_position: " + std::to_string(entity));
		return nullptr;
	}

	return &(it->second);
}

void PhysicsEngine::remove_entity(int entity) {
	positions.erase(entity);
	collidable.erase(entity);
	raycast.erase(entity);
	triggers.erase(entity);
	detach_position(entity);
}

void PhysicsEngine::detach_position(int entity) {
	auto& attached = attached_entities;
	auto it = std::remove_if(attached.begin(), attached.end(), [entity](auto& attachment) {
		return attachment.entity == entity;
	});
	
	attached.erase(it, attached.end());
}

PhysicsEngine& get_physics_engine() {
	static PhysicsEngine engine;
	return engine;
}

void InteractionSystem::add_interactable(int entity, Interactable interactable) {
	interactables[entity] = interactable;
}

Interactable* InteractionSystem::get_interactable(int entity) {
	auto it = interactables.find(entity);
	if (it == interactables.end()) {
		return nullptr;
	}

	return &(it->second);
}

void InteractionSystem::remove_entity(int entity) {
	interactables.erase(entity);
	if (interacted_with == entity) entity = -1;
}


void InteractionSystem::update(float dt) {
	// Reset shit from the previous frame
	interacted_with = -1;

	auto& entity_manager = get_entity_manager();
	int player = entity_manager.player;

	if (player < 0) return;
	if (!check_for_interactions) return;

	auto& physics = get_physics_engine();

	Center_Box vision;
	vision.origin = *physics.get_position(player);
	vision.origin += player_vision.offset;
	vision.extents += player_vision.extents;

	for (const auto& [entity, interactable] : interactables) {
		auto position = physics.get_position(entity);
		if (!position) {
			std::string message = "@no_physics_for_interactable, ";
			message += std::to_string(entity);
			tdns_log.write(message);
			continue;
		}

		Center_Box interactable_box;
		interactable_box.origin = *position + interactable.offset;
		interactable_box.extents = interactable.extents;
		
		glm::vec2 penetration;
		if (are_boxes_colliding(vision, interactable_box, penetration)) {
			interacted_with = interactable.entity;
			tdns_log.write("found interaction, " + std::to_string(interacted_with));

			// It only makes sense to interact with one thing at a time, so if
			// their bounding boxes happen to overlap, choose 1st arbitrarily.
			break;
		}
	}

	check_for_interactions = false;
}

InteractionSystem& get_interaction_system() {
	static InteractionSystem system;
	return system;
}
