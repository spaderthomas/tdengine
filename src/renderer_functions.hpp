void Renderer::draw(Graphic_Component* gc, Position_Component* pc) {
	Render_Element info = { gc, pc };
	render_list.push_back(info);
}
void Renderer::render_for_frame() {
	bind_sprite_buffers();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // Verts always the same (a square)
	glEnableVertexAttribArray(0);

	textured_shader.begin();
	textured_shader.set_int("sampler", 0);
	glm::vec2 camera_pos = glm::vec2(camera.x * GLSCR_TILESIZE_X, -1.f * camera.y * GLSCR_TILESIZE_Y);

	// Algorithm:
	// Sort by Z-position (as if you were going to do the Painter's algorithm
	// Z positions are integral (since 2D), so just collect elements of each Z-type into collections
	// Sort these collections by Y-axis (so overlap works properly)
	sort(render_list.begin(), render_list.end(), [](const Render_Element& a, const Render_Element& b) {return a.gc->z < b.gc->z; });
	int z = numeric_limits<int>::min();
	vector<vector<Render_Element>> depth_sorted_render_elements;
	for (auto& render_element : render_list) {
		if (render_element.gc->z > z) {
			vector<Render_Element> new_depth_level;
			depth_sorted_render_elements.push_back(new_depth_level);
			z = render_element.gc->z;
		}
		depth_sorted_render_elements.back().push_back(render_element);
	}

	for (auto& depth_level_render_elements : depth_sorted_render_elements) {
		stable_sort(depth_level_render_elements.begin(), depth_level_render_elements.end(), 
			[](const Render_Element& a, const Render_Element& b) {
				return a.pc->screen_pos.y > b.pc->screen_pos.y; 
			});
		
		// Draw the correctly sorted elements for a depth level
		for (auto& render_element : depth_level_render_elements) {
			Sprite* sprite = render_element.gc->get_current_frame();
			if (sprite) {
				sprite->atlas->bind();

				// Point the texture coordinates to this sprite's texcoords
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), sprite->tex_coord_offset);
				glEnableVertexAttribArray(1);
				
				SRT transform = SRT::no_transform();
				transform.scale = render_element.pc->scale;
				transform.translate = glm::vec3(gl_from_screen(render_element.pc->screen_pos), 0.f);
				transform.translate -= glm::vec3(camera_pos, 0.f);
				auto transform_mat = mat3_from_transform(transform);
				textured_shader.set_mat3("transform", transform_mat);

				textured_shader.check();
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	}

	textured_shader.end();
	render_list.clear();
}
