struct Level {
	std::string name;
	std::vector<EntityHandle> tiles;
	std::vector<EntityHandle> entities;

	EntityHandle get_tile(int x, int y);
	void set_tile(EntityHandle handle, int x, int y);
	EntityHandle get_first_matching_entity(std::string name);
	EntityHandle erase_first_matching_entity(std::string name);
	void clear_entities();
	void load_entities(TableNode* entities_table);
	void create_or_add_entities(TableNode* entities_table);
	void draw();
	void save();
	void load();
};

std::unordered_map<std::string, Level*> levels;

bool is_valid_level_name(std::string name);
void swap_level(Layer* layer, std::string name);
