void Renderer::draw(Graphic_Component* gc, Position_Component* pc, EntityHandle e, Render_Flags flags) {
	Render_Element info = { gc, pc, e, flags };
	render_list.push_back(info);
}
void Renderer::render_for_frame() {
	bind_sprite_buffers();
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // Verts always the same (a square)
	glEnableVertexAttribArray(0);

	Shader* shader = &textured_shader;
	shader->begin();
	shader->set_int("sampler", 0);

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

	// Main render loop
	glm::vec2 camera_translation = magnitude_gl_from_screen(camera.offset);
	for (auto& depth_level_render_elements : depth_sorted_render_elements) {
		stable_sort(depth_level_render_elements.begin(), depth_level_render_elements.end(), 
			[](const Render_Element& a, const Render_Element& b) {
				return a.pc->world_pos.y > b.pc->world_pos.y; 
			});
		
		// Draw the correctly sorted elements for a depth level
		for (auto& render_element : depth_level_render_elements) {
			// Swap shader based on flags
			if (render_element.flags & Render_Flags::Highlighted) {
				if (shader != &highlighted_shader) {
					shader->end();
					shader = &highlighted_shader;
					shader->begin();
					shader->set_int("sampler", 0);
				}
			}
			else {
				if (shader != &textured_shader) {
					shader->end();
					shader = &textured_shader;
					shader->begin();
					shader->set_int("sampler", 0);
				}
			}

			Sprite* sprite = render_element.gc->get_current_frame();
			if (sprite) {
				sprite->atlas->bind();

				// Point the texture coordinates to this sprite's texcoords
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), sprite->tex_coord_offset);
				glEnableVertexAttribArray(1);
				
				SRT transform = SRT::no_transform();
				transform.scale = render_element.gc->scale;
				transform.translate = gl_from_screen(render_element.pc->world_pos);
				transform.translate -= camera_translation;
				auto transform_mat = mat3_from_transform(transform);
				shader->set_mat3("transform", transform_mat);

				shader->check();
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	}

	shader->end();
	render_list.clear();


	// Finally, render all the primitives on top of the screen in the order they were queued.
	for (auto& draw_func : primitives) {
		draw_func();
	}
	primitives.clear();
}
