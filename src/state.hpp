enum State_Ternary {
	ST_FALSE,
	ST_TRUE,
	ST_UNINITIALIZED
};


struct State_System {
	sqlite3* db_conn;

	void init() {
		fox_assert(!sqlite3_open(db_dir.c_str(), &db_conn));
	}

	void sql_wrapper(const char* query, sqlite3_stmt** statement, bool finalize) {
		const char* query_end = query + strlen(query);
		sqlite3_prepare(db_conn, query, 1024, statement, &query_end);
		sqlite3_step(*statement);
		if (finalize) sqlite3_finalize(*statement);
	}
	string entity_state(string entity_name) {
		string query = "select * from entity_state where name = '" + entity_name + "'";
		sqlite3_stmt* statement;
		sql_wrapper(query.c_str(), &statement, false);
		const char* c_state = (const char*)sqlite3_column_text(statement, db_schema["entity_state"]["state"]);
		fox_assert(c_state);
		return string(c_state);
	}

	void update_state(string var, bool value) {
		// Update the value of the variable in the database
		string value_str = value ? "1" : "0";
		string query = "update state set value = " + value_str;
		query += " where name = '" + var + "'";
		sqlite3_stmt* sql_statement = nullptr;

		sql_wrapper(query.c_str(), &sql_statement, true);

		// Propagate the changes out to entities
		sql_wrapper("select * from entity_state", &sql_statement, false);
		do {
			string entity_name = (const char*)sqlite3_column_text(sql_statement, db_schema["entity_state"]["name"]);
			string state = (const char*)sqlite3_column_text(sql_statement, db_schema["entity_state"]["state"]);

			sol::table transitions = Lua.state["entity"][entity_name]["State_Machine"][state]["transitions"];
			for (auto& kvp : transitions) {
				sol::table transition = kvp.second;
				sol::table vars = transition["vars"];
				bool do_transition = true;

				for (auto& kvp2 : vars) {
					string varname = kvp2.first.as<string>();
					State_Ternary value_for_transition = kvp2.second.as<bool>() ? State_Ternary::ST_TRUE : State_Ternary::ST_FALSE;

					query = "select * from state where name = '" + varname + "'";
					sqlite3_stmt* sql_statement2 = nullptr;
					sql_wrapper(query.c_str(), &sql_statement2, false);
					State_Ternary state_val = (State_Ternary)sqlite3_column_int(sql_statement2, db_schema["state"]["value"]);
					if (state_val != value_for_transition) {
						do_transition = false;
						break;
					}
				}

				if (do_transition) {
					string new_state = transition["next_state"];
					query = "update entity_state set state = '" + new_state + "' where name = '" + entity_name + "'";
					sqlite3_stmt* sql_statement3;
					sql_wrapper(query.c_str(), &sql_statement3, true);

					for (auto& entity : game.active_level->entities) {
						if (entity->lua_id == entity_name) {
							def_get_cmp(tc, entity.deref(), Task_Component);
							sol::table new_task = Lua.entity_table()[entity_name]["scripts"][new_state];
							tc->change_task(new_task);
						}
					}
					continue;
				}
			}
		} while (sqlite3_step(sql_statement) != SQLITE_DONE);
	}
};
State_System state_system;