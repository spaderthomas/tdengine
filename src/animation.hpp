struct Animation : Asset {
	vector<Sprite*> frames;
	int icur_frame = -1;

	void draw(GLenum mode, SRT transform) {
		if (icur_frame > -1) {
			Sprite* cur_frame = frames[icur_frame];
			renderer.draw_sprite(cur_frame, transform);
		}
		else {
			string msg = "Invalid frame index (less than 0!). Animation name: " + this->name;
			cout << msg;
			tdns_log.write(msg.c_str());
		}
	}

	void add_frame(Sprite* frame) {
		frames.push_back(frame);
	}

	void next_frame() {
		icur_frame = (icur_frame + 1) % frames.size();
	}
	void reset() {
		icur_frame = 0;
	}

};