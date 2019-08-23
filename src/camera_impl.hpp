void Camera::update(float dt) {
	if (following) {
		def_get_cmp(follow_pc, following.deref(), Position_Component);
		offset = follow_pc->world_pos;
		offset += glm::vec2{-.5, -.5};
	}
}
