// @note @spader 9/3/2019: Fucking unhygienic enums. Can't use Console in an enum because I have a class
// that is also named Console. 
namespace Log_Flags {
	uint8_t Console = 1;
	uint8_t File    = 2;
	uint8_t Default = 3; // 1 | 2
};

struct Log {
	ofstream log_stream;
	
	void init();
	void write(string message, uint8_t flags = Log_Flags::Default);
	
};
Log tdns_log;

void Log::init() {
	log_stream.open(root_dir + "log.txt", ofstream::out | ofstream::trunc);
}
void Log::write(string message, uint8_t flags) {
	if (flags & Log_Flags::Console)
		cout << message << endl;
	if (flags & Log_Flags::File)
		log_stream << message << endl;
}
