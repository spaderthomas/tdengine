struct SRT {
	glm::vec2 translate;
	glm::vec2 scale;
	float rad_rot;
	
	static SRT no_transform() {
		SRT t;
		t.scale = glm::vec2(1.0f);
		t.rad_rot = 0.f;
		t.translate = glm::vec2(0.0f);
		return t;
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
