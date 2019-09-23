void Camera::update(float dt) {
	if (!panning && following) {
		def_get_cmp(follow_pc, following.deref(), Position_Component);
		offset = follow_pc->world_pos;
		offset += glm::vec2{-.5, -.5};
	}

	// Pan the camera towards what it's following
	if (panning && following) {
		auto world_offset = offset;
		world_offset -= glm::vec2{-.5, -.5};
		if_component(pc, following, Position_Component) {
			// If we're at the pan target, stop panning and snap to it next frame
			if (vec_almost_equals(world_offset, pc->world_pos)) {
				panning = false;
			}
			// Otherwise, do a pan
			else {
				// @note @spader 9/3/2019: Make these variables in a scripted file
				if (world_offset.x < pc->world_pos.x) {
					offset.x += CAMERA_ADJUST_AMOUNT;
				} else {
					offset.x -= CAMERA_ADJUST_AMOUNT;
				}

				if (world_offset.y < pc->world_pos.y) {
					offset.y += CAMERA_ADJUST_AMOUNT;
				} else {
					offset.y -= CAMERA_ADJUST_AMOUNT;
				}
			}
		}	
	}
}

void Camera::pan_and_follow(EntityHandle entity) {
	panning = true;
	following = entity;
}

void Camera::follow(EntityHandle entity) {
	following = entity;
}

bool Camera::is_at_entity(EntityHandle entity) {
	if (!entity) return true;
	
	auto world_offset = offset;
	world_offset -= glm::vec2{-.5, -.5};
	if_component(pc, entity, Position_Component) {
		// Finish the action if it is
		if (vec_almost_equals(world_offset, pc->world_pos)) {
			return true;
		}
	}
	
	return false;
}
