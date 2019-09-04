struct Entity_Tree {
	string dir;
	vector<pool_handle<Entity>> entities;
	vector<Entity_Tree*> children;
	int size = 0;
	
	static Entity_Tree* create(string dir);
	pool_handle<Entity> find(string name);
}; 

enum Editor_State {
	IDLE,
	INSERT,
	EDIT,
	DRAG,
	RECTANGLE_SELECT
};
enum Selection_Kind {
	NONE,
	TILE,
	ENTITY
};
struct Editor : Layer {
	// Stores tiles in a tree structure for good display purposes.
	Entity_Tree* tile_tree;
	void draw_tile_tree(Entity_Tree* root);
	int draw_tile_tree_recursive(Entity_Tree* root, int unique_btn_index);
	Editor_State state = IDLE;
	Selection_Kind kind = NONE;
	TaskEditor task_editor;
	
	// State for moving things around cleanly
	glm::ivec2 last_grid_drawn = { 0, 0 };
	glm::vec2 top_left_drag = { 0.f, 0.f };
	glm::vec2 smooth_drag_offset = { 0.f, 0.f };
	
	// Grid state
	bool show_grid = false;
	bool last_show_grid = false;
	bool snap_to_grid = false;
	bool last_snap_to_grid = false;
	
	// State about selected thing
	EntityHandle selected;
	void translate();
	void delete_selected();
	void draw_component_editor();
	
	// Undo 
	vector<function<void()>> action_stack;
	void undo_action();
	vector<function<void()>> mark_stack;
	void undo_mark();
	
	
	// Big stuff
	void reload_assets();
	void reload() override;
	void exec_console_cmd(char* cmd) override;
	void update(float dt) override;
	void render() override;
	void init() override;
};
Editor editor;