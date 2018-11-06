struct Animation {
	string name;
	vector<Sprite*> frames;
	int icur_frame = -1;

	void next_frame();
};
