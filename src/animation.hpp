struct Animation : Asset {
	vector<Sprite*> frames;
	int icur_frame = -1;

	Sprite* get_active_sprite() {
		if (icur_frame > -1) {
			return frames[icur_frame];
		}
		else {
			string msg = "Invalid frame index (less than 0!). Animation name: " + this->name;
			cout << msg;
			tdns_log.write(msg.c_str());
		}
		return 0;
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