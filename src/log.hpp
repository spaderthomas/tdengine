struct {
	ofstream log_stream;
	
	void init() {
		log_stream.open(root_dir + "log.txt", ofstream::out | ofstream::trunc);
	}

	void write(string message) {
		cout << message << endl;
		log_stream << message << endl;
	}
	
} tdns_log;
