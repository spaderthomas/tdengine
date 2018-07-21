struct FSM {
	vector<string> states;
	string current_state;

	virtual void set_to_initial_state() {};
	virtual void update(bool* updated_variable) {};
};

struct {
	unordered_map<bool*, vector<FSM*>> watch_table;

	bool boon_has_interacted;

	void setup() {
		boon_has_interacted = false;
	}

	void update_variable(bool* variable, bool value) {
		*variable = value;

		vector<FSM*>& watchers = watch_table[variable];
		for (auto& fsm : watchers) {
			fsm->update(variable);
		}
	}

	void register_watcher(bool* watched_variable, FSM* watcher) {
		watch_table[watched_variable].push_back(watcher);
	}
} knowledge_base;

struct : FSM {
	void set_to_initial_state() override {
		states.clear();
		states.push_back("never_interacted");
		states.push_back("have_interacted");

		current_state = "never_interacted";

		knowledge_base.register_watcher(&knowledge_base.boon_has_interacted, this);
	}

	void update(bool* updated_variable) override {
		if (current_state == "never_interacted") {
			if (updated_variable == &knowledge_base.boon_has_interacted) {
				if (*updated_variable == true) {
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