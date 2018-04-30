struct Animation : Asset {
	vector<string> frame_ids;
	int icur_frame = -1;

	void bind() {
		string cur_frame_id = frame_ids[icur_frame];
		Texture* cur_frame = asset_table.get_texture(cur_frame_id);
		cur_frame->bind();
	}

	void add_frame(string frame_id) {
		frame_ids.push_back(frame_id);
	}
	void add_frame(Texture* frame) {
		frame_ids.push_back(frame->name);
	}

	void next_frame() {
		icur_frame = (icur_frame + 1) % frame_ids.size();
	}
	void reset() {
		icur_frame = 0;
	}

};