// @note @spader 9/3/2019: Fucking unhygienic enums. Can't use Console in an enum because I have a class
// that is also named Console.
// @note @spader 2/17/2020: use enum class bro
namespace Log_Flags {
	uint8_t Console = 1;
	uint8_t File    = 2;
	uint8_t Default = 3; // 1 | 2
};

struct Log {
	std::ofstream log_stream;
	
	void init();
	void write(std::string message, uint8_t flags = Log_Flags::Default);
	
};
Log tdns_log;

void Log::init() {
	log_stream.open(root_dir + "log.txt", std::ofstream::out | std::ofstream::trunc);
}
void Log::write(std::string message, uint8_t flags) {
	if (flags & Log_Flags::Console)
		std::cout << message << std::endl;
	if (flags & Log_Flags::File)
		log_stream << message << std::endl;
}
