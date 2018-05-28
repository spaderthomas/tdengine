struct {
	sol::state state;

	void init() {
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io);
		state.script_file("../../src/scripts/tree.lua");
		state.script_file("../../src/scripts/boon.lua");
	}
} Lua;