// Small quality of life macros and typedefs
#define fox_max(a, b) (a) > (b) ? (a) : (b)
#define fox_min(a, b) (a) > (b) ? (b) : (a)
#define fox_for(iterName, iterCount) for (unsigned int iterName = 0; iterName < (iterCount); ++iterName)
#define fox_iter(iter_name, container) for (auto iter_name = (container).begin(); (iter_name) != (container).end(); (iter_name)++)
#define EXIT_IF_ERROR(return_code) if ((return_code)) { return -1; }
#define rand_float(max) (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max)))

typedef unsigned int uint;
typedef int8_t int8;
typedef int32_t int32;
typedef int64_t int64;
typedef unsigned char tdbyte;

#if  defined(__APPLE__) || defined(__linux__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#define __stdcall
#endif

//Assert
#ifdef _MSC_VER
#	ifdef assert
#		undef assert
#	endif
#	define fox_assert(expr) if (!(expr)) _CrtDbgBreak()
#else
#	define fox_assert(expr) assert(expr)
#endif

// C++ I love you, but you're bringing me down
template <typename T, bool = std::is_enum<T>::value>
struct is_flag;

template <typename T>
struct is_flag<T, true> : std::false_type { };

#define ENABLE_ENUM_FLAG(EnumType) template <> struct is_flag<EnumType> : std::true_type {};

template <typename T, typename std::enable_if<is_flag<T>::value>::type* = nullptr>
T operator |(T lhs, T rhs) {
    using u_t = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<u_t>(lhs) | static_cast<u_t>(rhs));
}

template <typename T, typename std::enable_if<is_flag<T>::value>::type* = nullptr>
T operator &(T lhs, T rhs) {
    using u_t = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<u_t>(lhs) & static_cast<u_t>(rhs));
}

template <typename T, typename std::enable_if<is_flag<T>::value>::type* = nullptr>
bool has_flag(T lhs, T rhs) {
    using u_t = typename std::underlying_type<T>::type;
    u_t has = static_cast<u_t>(lhs) & static_cast<u_t>(rhs);
	return has != 0;
}

typedef int GLFW_KEY_TYPE;
typedef GLFW_KEY_TYPE key_t;
#define GLFW_KEY_CONTROL 349
#define GLFW_KEY_SUPER 350
#define GLFW_KEY_SHIFT 351
#define GLFW_KEY_ALT 352

#define ASCII_OPAREN     40
#define ASCII_CPAREN     41
#define ASCII_QMARK      63
#define ASCII_UNDERSCORE 95

#define INPUT_MASK_NONE   (1 << 0)
#define INPUT_MASK_IMGUI  (1 << 1)
#define INPUT_MASK_EDITOR (1 << 2)
#define INPUT_MASK_GAME   (1 << 3)
#define INPUT_MASK_PLAYER (1 << 4)
#define INPUT_MASK_ALL    (0xF)

// STL extensions 
template<typename vec_type>
void concat(std::vector<vec_type>& append_to, std::vector<vec_type>& append_from) {
	append_to.insert(append_to.end(), append_from.begin(), append_from.end());
}

std::vector<std::string> split(const std::string &str, char delim) {
	std::stringstream stream(str);
	std::string item;
	std::vector<std::string> tokens;
	while (getline(stream, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

void string_replace(std::string& str, std::string from, std::string to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

bool does_string_contain_substr(std::string& str, std::string& substr) {
	return str.find(substr) != std::string::npos;
}

#define tdns_find(vector, item) (find((vector).begin(), (vector).end(), (item)) != (vector).end()) 
#define are_strings_equal(a, b) (!(a).compare((b)))


glm::vec2 tdns_normalize(glm::vec2 vec) {
	if (vec.x == 0.f && vec.y == 0.f) {
		return vec;
	}
	
	return glm::normalize(vec);
}
glm::vec3 tdns_normalize(glm::vec3 vec) {
	if (vec.x == 0.f && vec.y == 0.f && vec.z == 0.f) {
		return vec;
	}
	
	return glm::normalize(vec);
}

#define TDENGINE_FLOATEQ_EPSILON .00005
bool float_almost_equals(float a, float b) {
	return glm::abs(a - b) < TDENGINE_FLOATEQ_EPSILON;
}
bool vec_almost_equals(glm::vec2 vec, glm::vec2 target) {
	return glm::length(vec - target) < TDENGINE_FLOATEQ_EPSILON;
}

glm::vec2 vec_divide(glm::vec2 vec, float by) {
	return glm::vec2 { vec.x / by, vec.y / by };
}


// Colors
namespace Colors {
	glm::vec4 Red = glm::vec4(1.f, 0.f, 0.f, 1.f);
	glm::vec4 Green = glm::vec4(0.f, 1.f, 0.f, 1.f);
	glm::vec4 Blue = glm::vec4(0.f, 0.f, 1.f, 1.f);
	glm::vec4 Brown = glm::vec4(173.f / 255.f, 133.f / 255.f, 74.f / 255.f, 1.f);
	glm::vec4 Black = glm::vec4(0.f, 0.f, 0.f, 1.f);
	glm::vec4 White4 = glm::vec4(1.f, 1.f, 1.f, 1.f);
	glm::vec3 TextWhite = glm::vec3(1.f, 1.f, 1.f);
	glm::vec3 TextHighlighted = glm::vec3(1.f, 0.f, 0.f);
}
#define ImGuiColor_Red ImVec4(1.f, 0.f, 0.f, 1.f)
#define ImGuiColor_Green ImVec4(0.f, 1.f, 0.f, 1.f)
	
// Shape primitives
std::vector<float> triangle_verts = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f,  0.5f, 0.0f
};
std::vector<uint> triangle_indices = {
	0, 1, 2,
};
std::vector<float> triangle_tex_coords = {
	0.0f, 0.0f,
	1.f, 0.0f,
	0.5f, 1.0f,
};

std::vector<float> square_verts = {
	1.f,  1.f,
	1.f, -1.f,
	-1.f, -1.f,
	-1.f,  1.f
};
std::vector<uint> square_indices = {
	0, 1, 2,
	2, 0, 3
};
std::vector<float> square_tex_coords = {
	1.f, 1.f,
	1.f, 0.0f,
	0.f, 0.f,
	0.f, 1.f,
};
GLvoid* square_tex_coords_offset;

std::vector<float> line_verts = {
	0.f, 0.f, 1.f,
	1.f, 1.f, 1.f,
};
std::vector<uint> line_indices = {
	0, 1,
};

GLFWwindow* g_window;

/*
Some unit definitions:
GL coordinates have 
 the leftmost coordinate at -1, 
 the rightmost at +1,
 the bottommost at -1,
 the topmost at +1
 
Screen coordinates have
 the leftmost coordinate at 0,
 the rightmost at +1,
 the bottommost at 0,
 the topmost at +1
 
Grid coordinates have
 the leftmost coordinate at 0,
 the rightmost at +1,
 the bottommost at 0,
 the topmost at +1
 
Pixel coordinates have
 the leftmost coordinate at 0,
 the rightmost at SCREEN_X,
 the bottommost at 0,
 the topmost at SCREEN_Y
 
 
 
Below are all the conversion functions. Using them and proper units is a bit verbose, 
but is worth it to save the confusion of exchanging units (which is unavoidable). Also
of note is that since we're using GLM vectors, it's not really convenient to have 
conversions which use our typedefs, so conversions that take GLM vectors and single
points are defined
*/
typedef int32 pixel_unit;
typedef float screen_unit;
typedef float gl_unit;

float internal_resolution_width = 320;
float internal_resolution_height = 180;
float screen_x = 320;
float screen_y = 180;

// Screen size definitions
void use_640_360() {
	screen_x = 640.f;
	screen_y = 360.f;
	glfwSetWindowSize(g_window, 640, 360);
	glViewport(0, 0, (int)screen_x, (int)screen_y);
}
void use_720p() {
	screen_x = 1280.f;
	screen_y = 720.f;
	glfwSetWindowSize(g_window, 1280, 720);
	glViewport(0, 0, (int)screen_x, (int)screen_y);
}
void use_1080p() {
	screen_x = 1920.f;
	screen_y = 1080.f;
	glfwSetWindowSize(g_window, 1920, 1080);
	glViewport(0, 0, (int)screen_x, (int)screen_y);
}
void use_1440p() {
	screen_x = 2560.f;
	screen_y = 1440.f;
	glfwSetWindowSize(g_window, 2560, 1440);
	glViewport(0, 0, (int)screen_x, (int)screen_y);
}
void use_arbitrary_screen_size(int height, int width) {
	screen_x = static_cast<float>(width);
	screen_y = static_cast<float>(height);
	glViewport(0, 0, (int)screen_x, (int)screen_y);	
}
void change_window_size(std::string size) {
	if (size == "640") { use_640_360(); }
	else if (size == "720") { use_720p(); }
	else if (size == "1080") { use_1080p(); }
	else if (size == "1440") { use_1440p(); }
}

// Converting to GL units
glm::vec2 gl_from_screen(glm::vec2 screen_coords) {
	return glm::vec2(screen_coords.x * 2 - 1, screen_coords.y * 2 - 1);
}
gl_unit gl_from_screen(screen_unit s) {
	return s * 2 - 1;
}
glm::vec2 magnitude_gl_from_screen(glm::vec2 screen_coords) {
	// Moving .5 units in screen space is moving 1 unit in GL space
	// i.e. magnitudes are doubled
	return glm::vec2(screen_coords.x * 2, screen_coords.y * 2);
}
gl_unit magnitude_gl_from_screen(screen_unit s) {
	return s * 2;
}

// Converting to screen units
screen_unit magnitude_screen_from_gl(gl_unit u) {
	return (screen_unit)(u / 2);
}
screen_unit screen_x_from_px(pixel_unit px) {
	return px / screen_x;
}
screen_unit screen_y_from_px(pixel_unit px) {
	return px / screen_y;
}
glm::vec2 screen_from_px(glm::ivec2 px) {
	return glm::vec2(px.x / screen_x, px.y / screen_y);
}

// Converting to pixel units
glm::ivec2 px_coords_from_gl_coords(glm::vec2 gl_coords) {
	float y = (gl_coords.y + 1) / 2;
	float x = (gl_coords.x + 1) / 2;
	return glm::ivec2(floor(x * screen_x), floor(y * screen_y));
}

glm::ivec2 px_from_screen(glm::vec2 screen) {
	return glm::ivec2(floor(screen.x * screen_x), floor(screen.y * screen_y));
}



bool is_alphanumeric(std::string& str) {
	auto is_numeric = [](char c) -> bool { return c >= '0' && c <= '9'; };
	auto is_alpha = [](char c) -> bool { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); };
	
	for (unsigned int ichar = 0; ichar < str.size(); ichar++) {
		char c = str.at(ichar);
		if (!(is_numeric(c) || is_alpha(c))) {
			return false;
		}
	}
	
	return true;
}

// @hack There is a better way to do this
std::string g_dialogue_font;
std::string g_dialogue_font_path;
int         g_dialogue_font_size;
std::string g_editor_font;
std::string g_editor_font_path;
int         g_editor_font_size;

// Global options
bool debug_show_aabb = false;
bool are_updates_paused = false;
bool print_framerate = false;
bool show_imgui_demo = false;
bool show_console = false;
bool send_kill_signal = false;
bool step_mode = false;

// These options are loaded from Lua
enum class TextureMode {
	Atlas                = 0,
	SingleWithHotloading = 1,
	AtlasWithHotloading  = 2,
};
TextureMode g_texture_mode;

// Set this string from a script, and we will pick up a new layout next tick
const char* layout_to_load = nullptr; 

float framerate = 0.f;

/* Random shit */
void __stdcall gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void *userParam) {
	(void)userParam;
	
	switch (id) {
		case 131169: // The driver allocated storage for renderbuffer
		return;
		case 131185: // glBufferData
		return;
		case 481131: // buffer info
		return;
		case 131184: // buffer info
		return;
		case 131204: // Weird base level inconsistent bug. It may be a driver bug.
		return;
	}
	
	std::string debug_msg;
	debug_msg += "OpenGL Debug Message: ";
	debug_msg += "\nSource: ";
	switch (source) {
		case GL_DEBUG_SOURCE_API:
		debug_msg += "API";
		break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		debug_msg += "Window System";
		break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
		debug_msg += "Shader Compiler";
		break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
		debug_msg += "Third Party";
		break;
		case GL_DEBUG_SOURCE_APPLICATION:
		debug_msg += "Application";
		break;
		case GL_DEBUG_SOURCE_OTHER:
		debug_msg += "Other";
		break;
	}
	
	debug_msg += "\nType: ";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
		debug_msg += "Error";
		break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		debug_msg += "Deprecated Behaviour";
		break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		debug_msg += "Undefined Behaviour";
		break;
		case GL_DEBUG_TYPE_PORTABILITY:
		debug_msg += "Portability";
		break;
		case GL_DEBUG_TYPE_PERFORMANCE:
		debug_msg += "Performance";
		break;
		case GL_DEBUG_TYPE_MARKER:
		debug_msg += "Marker";
		break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
		debug_msg += "Push Group";
		break;
		case GL_DEBUG_TYPE_POP_GROUP:
		debug_msg += "Pop Group";
		break;
		case GL_DEBUG_TYPE_OTHER:
		debug_msg += "Other";
		break;
	}
	
	debug_msg += "\nID: ";
	debug_msg += std::to_string(id);
	
	debug_msg += "\nSeverity: ";
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
		debug_msg += "High";
		break;
		case GL_DEBUG_SEVERITY_MEDIUM:
		debug_msg += "Medium";
		break;
		case GL_DEBUG_SEVERITY_LOW:
		debug_msg += "Low";
		break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
		debug_msg += "Notification";
		break;
	}
	
	debug_msg += "\nGL message: " + std::string(message);
	debug_msg += "\n\n";
	tdns_log.write(debug_msg, Log_Flags::File);
}

float seconds_per_update = 1.f / 60.f;
double pi = 3.14159;

glm::vec2 pairwise_max(glm::vec2 a, glm::vec2 b) {
	return {
		fox_max(a.x, b.x),
		fox_max(a.y, b.y)
	};
}

// amp * sin(period * k - phase_shift)
struct Sine_Func {
	float amp = 1.f;
	float period = 1.f;
	float phase_shift = 1.f;
	
	float eval_at(float point);
};

float Sine_Func::eval_at(float point) {
	return amp * glm::sin(period * point - phase_shift);
}

size_t hash_label(const char* label) {
	constexpr size_t prime = 31;
	
	size_t result = 0;
	size_t len = strlen(label);
	for (int i = 0; i < len; i++) {
        result = label[i] + (result * prime);
    }
    return result;
}

struct InputTextBuffer {
	char* data;
	int size;
};


std::unordered_map<size_t, InputTextBuffer>& input_text_buffers() {
	static std::unordered_map<size_t, InputTextBuffer> map;
	return map;
}

void add_input_text_buffer(const char* label, size_t size) {
	auto& buffers = input_text_buffers();

	size_t hash = hash_label(label);
	if (!buffers.count(hash)) {
		InputTextBuffer buffer;
		buffer.data = (char*)calloc(size, sizeof(char));
		buffer.size = size;
		buffers[hash] = buffer;
	}	
}

InputTextBuffer* get_input_text_buffer(const char* label) {
	auto& buffers = input_text_buffers();
	
	size_t hash = hash_label(label);
	if (!buffers.count(hash)) return nullptr;
	return &buffers[hash];
}

void clear_input_text_buffer(const char* label) {
	auto& buffers = input_text_buffers();

	size_t hash = hash_label(label);
	if (buffers.count(hash)) {
		auto buffer = &buffers[hash];
		memset(buffer->data, 0, strlen(buffer->data));
	}
}

void init_imgui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(g_window, false);
	
	auto& imgui = ImGui::GetIO();
	imgui.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	auto imgui_font = imgui.Fonts->AddFontFromFileTTF(g_editor_font_path.c_str(), g_editor_font_size);

	imgui.IniFilename = nullptr;

	// Engine will pick this up on the first tick (before ImGui renders, so no flickering)
	layout_to_load = "default";
}

void load_imgui_layout() {
	// if (!layout_to_load) return;

	// // Get the platform dependent path to the INI file. You can't use the virtual
	// // filesystem, because you must pass the path to ImGui
	// std::string layout;
	// layout = layout + "layouts/" + layout_to_load + ".ini";
	// layout = physfs_file_path(layout);
	
	// tdns_log.write("@imgui_layout_load::" + layout);
	// ImGui::LoadIniSettingsFromDisk(layout.c_str());

	// layout_to_load = nullptr;
}

template <typename F>
struct Defer {
    Defer( F f ) : f( f ) {}
    ~Defer( ) { f( ); }
    F f;
};

template <typename F>
Defer<F> makeDefer( F f ) {
    return Defer<F>( f );
};

#define __defer( line ) defer_ ## line
#define _defer( line ) __defer( line )

struct defer_dummy { };
template<typename F>
Defer<F> operator+( defer_dummy, F&& f )
{
    return makeDefer<F>( std::forward<F>( f ) );
}

#define defer auto _defer( __LINE__ ) = defer_dummy( ) + [ & ]( )

using FileChangedCallback = std::function<void()>;
using FileCreatedCallback = std::function<void(std::string)>;
struct FileWatcher {
	std::map<std::string, std::filesystem::file_time_type> time_map;
	std::map<std::string, FileChangedCallback> action_map;
	std::map<std::string, FileCreatedCallback> dir_watchers;

	void watch(std::string file, FileChangedCallback on_change) {
		if (file.find('#') != std::string::npos) return;
		tdns_log.write("@watch_file: " + file, Log_Flags::File);
		time_map[file] = std::filesystem::last_write_time(file);
		action_map[file] = on_change;
	}

	void watch_dir(std::string dir, FileCreatedCallback on_create) {
		if (!std::filesystem::is_directory(dir)) return;

		tdns_log.write("@watch_dir: " + dir, Log_Flags::File);
		dir_watchers[dir] = on_create;
		for (directory_iterator it(dir); it != directory_iterator(); it++) {
			if (it->is_directory()) {
				watch_dir(it->path().string(), on_create);
			} else {
				auto file_path = it->path().string();
				if (file_path.find('#') != std::string::npos) continue;
				time_map[file_path] = std::filesystem::last_write_time(file_path);
			}
		}
	}

	void update() {
		// Run through each regular file that we're watching
		std::error_code error;
		std::vector<std::string> invalid;
		for (auto& [path, last_known_write_time] : time_map) {
			auto last_write_time = std::filesystem::last_write_time(path, error);
			if (last_write_time == std::filesystem::file_time_type::min()) {
				invalid.push_back(path);
				continue;
			}
			if (last_known_write_time < last_write_time) {
				time_map[path] = last_write_time;
				action_map[path]();
			}
		}

		for (const auto& path : invalid) {
			tdns_log.write("Erasing path from filesystem watcher: " + path);
			time_map.erase(path);
			action_map.erase(path);
		}

		// Check watched directories for new files
		for (auto& [dir, on_create] : dir_watchers) {
			check_dir_for_new_files(dir);
		}
	}

	void check_dir_for_new_files(std::string dir) {
		std::error_code error;
		for (directory_iterator it(dir); it != directory_iterator(); it++) {
			if (it->is_directory()) {
				check_dir_for_new_files(it->path().string());
			} else {
				auto file_path = it->path().string();

				// Fucking emacs autosave files
				if (file_path.find('#') != std::string::npos) continue;
				
				if (!time_map.count(file_path)) {
					time_map[file_path] = std::filesystem::last_write_time(file_path, error);
					auto& on_create = dir_watchers[dir];
					on_create(file_path);
				}
			}
		}			
	}
	
};
FileWatcher file_watcher;

// This is really stupid and slow. But the only time we use this is when we
// hotload images, so its performance is meaningless. Reason we have to do this
// is because when you export with Aseprite, it saves the PNG twice -- first time
// is invalid, second time is valid. 
bool dumb_is_valid_png(std::filesystem::path path) {
	if (!path.has_filename()) return false;
	if (path.extension() != ".png") return false;

	int x, y, n;
	auto data = stbi_load(path.string().c_str(), &x, &y, &n, 0);
	stbi_image_free(data);

	return data != nullptr;
}
