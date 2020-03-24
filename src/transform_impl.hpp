SRT SRT::no_transform() {
	SRT t;
	t.scale = glm::vec2(1.0f);
	t.rad_rot = 0.f;
	t.translate = glm::vec3(0.0f);
	return t;
}

glm::mat3 identity_mat3() {
	return glm::mat3(1.f);
}

// glm is column then row (array of columns_
glm::mat3 mat3_from_transform(SRT transform) {
	glm::mat3 scale_mat = identity_mat3();
	scale_mat[0][0] = transform.scale.x;
	scale_mat[1][1] = transform.scale.y;
	scale_mat[2][2] = 1.f;

	glm::mat3 rot_mat = identity_mat3();
	rot_mat[0][0] = glm::cos(transform.rad_rot);
	rot_mat[0][1] = glm::sin(transform.rad_rot);
	rot_mat[1][0] = -1.f * glm::sin(transform.rad_rot);
	rot_mat[1][1] = glm::cos(transform.rad_rot);

	glm::mat3 trans_mat = identity_mat3();
	trans_mat[2][0] = transform.translate.x;
	trans_mat[2][1] = transform.translate.y;
	trans_mat[2][2] = 1.f;

	return trans_mat * rot_mat * scale_mat;
}

// Single-tile scaled SRT centered at grid_pos. 
SRT srt_from_grid_pos(glm::ivec2 grid_pos) {
	SRT grid_pos_transform = SRT::no_transform();
	grid_pos_transform.scale = glm::vec2(SCR_TILESIZE_X, SCR_TILESIZE_Y);

	// Starting at -1, move grid_pos tiles forward
	grid_pos_transform.translate = glm::vec2(-1.f + grid_pos.x * GLSCR_TILESIZE_X, 1.f - grid_pos.y * GLSCR_TILESIZE_Y);

	// Since tile was centered before, it'll be halfway between the tile, so correct that
	grid_pos_transform.translate += glm::vec2(.5f * GLSCR_TILESIZE_X, -.5f * GLSCR_TILESIZE_Y);
	return grid_pos_transform;
}

glm::vec2 translation_from_grid_pos(glm::ivec2 grid_pos) {
	glm::vec2 translation = glm::vec2(-1.f + grid_pos.x * GLSCR_TILESIZE_X, 1.f - grid_pos.y * GLSCR_TILESIZE_Y);
	translation += glm::vec2(.5f * GLSCR_TILESIZE_X, -.5f * GLSCR_TILESIZE_Y);
	return translation;
}

glm::vec2 translation_from_px_pos(glm::vec2 px_pos) {
	glm::vec2 screen_coords = glm::vec2(px_pos.x / SCREEN_X, px_pos.y / SCREEN_Y);
	glm::vec2 gl_coords = gl_from_screen(screen_coords);
	return gl_coords;
}