struct FSM {
	vector<string> states;
	vector<string> watched_variables;
	string current_state;
	string name;

	virtual void set_to_initial_state() {};
	virtual void update(string updated_variable, bool value) {};
};

// Knowledge base holds a bunch of variables we need to figure out what to do 
// in stuff like dialogue trees. An FSM can add itself to a watch list for one of
// these variables, which lets it know to check for an update when the variable 
// gets updated
struct {
	unordered_map<string, vector<FSM*>> watch_table;
	unordered_map<string, bool> variables;

	void setup() {
		variables["boon_has_interacted"] = false;
	}

	void update_variable(string variable_name, bool value) {
		variables[variable_name] = value;
		vector<FSM*>& watchers = watch_table[variable_name];
		for (auto& fsm : watchers) {
			fsm->update(variable_name, value);
		}
	}

	void register_watcher(string watched_variable, FSM* watcher) {
		watch_table[watched_variable].push_back(watcher);
		watcher->watched_variables.push_back(watched_variable);
	}

	bool operator [] (string variable_name) {
		return variables[variable_name];
	}
} knowledge_base;


// FSMs
struct : FSM {
	void set_to_initial_state() override {
		name = "boon_dialogue";

		states.clear();
		states.push_back("never_interacted");
		states.push_back("have_interacted");

		current_state = "never_interacted";

		knowledge_base.register_watcher("boon_has_interacted", this);
	}

	void update(string updated_variable, bool value) override {
		if (current_state == "never_interacted") {
			if (updated_variable == "boon_has_interacted") {
				if (value == true) {
					current_state = "have_interacted";
					return;
				}
			}
		}
		else if (current_state == "have_interacted") {
			return;
		}
	}
} Boon_Dialogue_FSM;

// Only using this for ImGui debugging
vector<FSM*> all_fsm = {
	&Boon_Dialogue_FSM
};