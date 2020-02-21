void bind_sprite_buffers() {
	glBindVertexArray(Sprite::vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
}

void Animation::next_frame() {
	icur_frame = (icur_frame + 1) % frames.size();
	time_to_next_frame = 8.f / 60.f;
}

void Animation::add_frames(TableNode* frames_table) {
	std::vector<std::string> frames_to_add;
	for (uint frame_idx = 0; frame_idx < frames_table->assignments.size(); frame_idx++) {
		frames_to_add.push_back(tds_string2(frames_table, std::to_string(frame_idx)));
	}

	add_frames(frames_to_add);
}

void Animation::add_frames(std::vector<std::string>& frames_to_add) {
	for (auto& sprite_name : frames_to_add) {
		add_frame(sprite_name);
	}
}

void Animation::add_frame(std::string& sprite_name) {
	Sprite* frame = asset_table.get_asset<Sprite>(sprite_name);
	if (!frame) {
		tdns_log.write("Tried to add a frame to an animation, but couldn't find the sprite.");
		tdns_log.write("Requested sprite: " + sprite_name);
		tdns_log.write("Animation name" + this->name);
		return;
	}
	
	frames.push_back(frame);
}

bool Sprite::is_initialized() const{
	return (nullptr != this->atlas);
}
