struct Animation {
	string name;
	vector<Sprite*> frames;
	int icur_frame = -1;
	float time_to_next_frame = 8.f / 60.f;
	void next_frame();
};
