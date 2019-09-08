void bind_sprite_buffers() {
	glBindVertexArray(Sprite::vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
}

void Animation::next_frame() {
	icur_frame = (icur_frame + 1) % frames.size();
	time_to_next_frame = 8.f / 60.f;
}

bool Sprite::is_initialized() const{
	return (nullptr != this->atlas);
}
