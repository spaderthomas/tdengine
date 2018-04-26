struct {
	ofstream log_stream;
	
	void init() {
		log_stream.open("../../log.txt", ofstream::out | ofstream::trunc);
	}

	void write(const char* message) {
		while(*message) { log_stream.put(*message); message++; }
		log_stream.put('\n');
	}
	
} tdns_log;
