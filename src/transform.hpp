struct SRT {
	glm::vec3 translate;
	glm::vec2 scale;
	float rad_rot;
	
	static SRT no_transform() {
		SRT t;
		t.scale = glm::vec2(1.0f);
		t.rad_rot = 0.f;
		t.translate = glm::vec3(0.0f);
		return t;
	}

	void save(json& j) const {
		j["translate.x"] = translate.x;
		j["translate.y"] = translate.y;
		j["translate.z"] = translate.z;
		j["scale.x"] = scale.x;
		j["scale.y"] = scale.y;
		j["rad_rot"] = rad_rot;
	}
	
	void load(json& j) {
		translate.x = j["translate.x"];
		translate.y = j["translate.y"];
		translate.z = j["translate.z"];
		scale.x = j["scale.x"];
		scale.y = j["scale.y"];
		rad_rot = j["rad_rot"];
	}
};

glm::mat3 identity_mat3() {
	return glm::mat3(1.f);
}

// glm is column then row (array of columns_
glm::mat3 mat3_from_transform(SRT transform) {
	glm::mat3 scale_mat = identity_mat3();
	scale_mat[0][0] = transform.scale.x;
	scale_mat[1][1] = transform.scale.y;

	glm::mat3 rot_mat = identity_mat3();
	rot_mat[0][0] = glm::cos(transform.rad_rot);
	rot_mat[0][1] = glm::sin(transform.rad_rot);
	rot_mat[1][0] = -1.f * glm::sin(transform.rad_rot);
	rot_mat[1][1] = glm::cos(transform.rad_rot);

	glm::mat3 trans_mat = identity_mat3();
	trans_mat[2][0] = transform.translate.x;
	trans_mat[2][1] = transform.translate.y;

	return trans_mat * rot_mat * scale_mat;
}

// Single-tile scaled SRT centered at grid_pos. 
SRT srt_from_grid_pos(glm::ivec2& grid_pos) {
	SRT grid_pos_transform = SRT::no_transform();
	grid_pos_transform.scale = glm::vec2(SCR_TILESIZE_X, SCR_TILESIZE_Y);

	// Starting at -1, move grid_pos tiles forward
	grid_pos_transform.translate = glm::vec3(-1.f + grid_pos.x * GLSCR_TILESIZE_X, 1.f - grid_pos.y * GLSCR_TILESIZE_Y, 0.f);

	// Since tile was centered before, it'll be halfway between the tile, so correct that
	grid_pos_transform.translate += glm::vec3(.5f * GLSCR_TILESIZE_X, -.5f * GLSCR_TILESIZE_Y, 0.f);
	return grid_pos_transform;
}

glm::vec3 translation_from_grid_pos(glm::ivec2& grid_pos) {
	glm::vec3 translation = glm::vec3(-1.f + grid_pos.x * GLSCR_TILESIZE_X, 1.f - grid_pos.y * GLSCR_TILESIZE_Y, 0.f);
	translation += glm::vec3(.5f * GLSCR_TILESIZE_X, -.5f * GLSCR_TILESIZE_Y, 0.f);
	return translation;
}

glm::vec3 translation_from_px_pos(glm::vec2& px_pos) {
	glm::vec2 screen_coords = glm::vec2(px_pos.x / SCREEN_X, px_pos.y / SCREEN_Y);
	glm::vec2 gl_coords = gl_coords_from_screen_coords(screen_coords);
	return glm::vec3(gl_coords, 0);
}

bool is_point_inside_aligned_rectangle(glm::vec2 point, SRT rect_transform) {
	// @note: All these are really vec3s, but the conversion drops the Z coordinate like we want
	glm::vec2 top_left = mat3_from_transform(rect_transform) * screen_top_left;
	glm::vec2 bottom_right = mat3_from_transform(rect_transform) * screen_bottom_right;

	// These seem weird but it's because OpenGL has Y opposite of cartesian
	return (top_left.x < point.x &&
			top_left.y < point.y &&
			bottom_right.x > point.x &&
			bottom_right.y > point.y);

}