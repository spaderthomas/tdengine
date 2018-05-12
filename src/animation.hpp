struct Animation : Asset {
	vector<Texture*> frames;
	int icur_frame = -1;

	void bind() {
		if (icur_frame > -1) {
			Texture* cur_frame = frames[icur_frame];
			cur_frame->bind();
		}
		else {
			string msg = "Invalid frame index (less than 0!). Animation name: " + this->name;
			tdns_log.write(msg.c_str());
		}
	}

	void add_frame(Texture* frame) {
		frames.push_back(frame);
	}

	void next_frame() {
		icur_frame = (icur_frame + 1) % frames.size();
	}
	void reset() {
		icur_frame = 0;
	}

};