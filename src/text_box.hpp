struct Line_Set {
	std::vector<std::string> lines;
	int point = 0;
	int max_point = 0;
	
	void set_max_point();
	int count();
	void add(std::string line);
	std::string& operator[](int i);
};

struct Text_Box {
	std::string text;
	bool waiting = false;
	bool active = false;
	float scale = 1.f;
	float time_since_last_update = 0.0f;
	float time_per_update = 4 * seconds_per_update;
	
	std::vector<Line_Set> sets;
	uint index_current_line_set;
	
	void begin(std::string text);
	void update(float dt);
	void resume();
	void skip();
	void render();
	void reset_and_hide();
	bool is_current_set_displayed();
	bool is_done();
	Line_Set& current_set();
};

Text_Box& get_text_box();
