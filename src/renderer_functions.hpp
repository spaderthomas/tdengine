// We want to render in Y-descending order so everything overlaps properly
bool operator < (const Render_Element& elem, const Render_Element& elem2) {
	return elem.pc->transform.translate.y > elem2.pc->transform.translate.y;
}

void Renderer::draw(Graphic_Component* gc, Position_Component* pc) {
	Render_Element info = { gc, pc };
	render_list.push_back(info);
}
void Renderer::render_for_frame() {
	bind_sprite_buffers();
	textured_shader.bind();
	textured_shader.set_int("sampler1", 0);
	textured_shader.set_vec3("camera_translation", camera_pos);

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
				return a.pc->transform.translate.y > b.pc->transform.translate.y; 
			});

		for (auto& render_element : depth_level_render_elements) {
			Sprite* sprite = render_element.gc->get_current_frame();
			sprite->atlas->bind();

			sprite->bind();
			auto transform_mat = mat3_from_transform(render_element.pc->transform);
			textured_shader.set_mat3("transform", transform_mat);
			textured_shader.set_int("z", render_element.gc->z);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	render_list.clear();
}
