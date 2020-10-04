struct SRT {
	glm::vec2 translate;
	glm::vec2 scale;
	float rad_rot;
	
	static SRT no_transform();
};

glm::mat3 identity_mat3();
glm::mat3 mat3_from_transform(SRT transform);
glm::vec2 translation_from_px_pos(glm::vec2 px_pos);
