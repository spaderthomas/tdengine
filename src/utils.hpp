// Small quality of life macros and typedefs
#define fox_max(a, b) (a) > (b) ? (a) : (b)
#define fox_min(a, b) (a) > (b) ? (b) : (a)
#define fox_for(iterName, iterCount) for (unsigned int iterName = 0; iterName < (iterCount); ++iterName)
#define fox_iter(iter_name, container) for (auto iter_name = (container).begin(); (iter_name) != (container).end(); (iter_name)++)
#define EXIT_IF_ERROR(return_code) if ((return_code)) { return -1; }
#define rand_float(max) (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max)))

#define is_newline(c) ((c) == '\n' || (c) == '\r')
#define is_space(c) ((c) == ' ')

#define dont_care false

#define tdapi
	
typedef unsigned int uint;
typedef int8_t int8;
typedef int32_t int32;
typedef int64_t int64;
typedef unsigned char tdbyte;

#if  defined(__APPLE__) || defined(__linux__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#define NULL_ENTITY { -1, nullptr }

#if  defined(__APPLE__) || defined(__linux__)
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


typedef int GLFW_KEY_TYPE;
#define GLFW_KEY_CONTROL 349
#define GLFW_KEY_SUPER 350
#define GLFW_KEY_SHIFT 351

#define ASCII_OPAREN     40
#define ASCII_CPAREN     41
#define ASCII_QMARK      63
#define ASCII_UNDERSCORE 95

#define INPUT_MASK_NONE (1 << 0)
#define INPUT_MASK_IMGUI (1 << 1)
#define INPUT_MASK_EDITOR (1 << 2)
#define INPUT_MASK_GAME (1 << 3)
#define INPUT_MASK_ALL (0xF)

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

// @note @spader 9/4/2019 Realllllllyyyyyyy need a better way of making paths good
#ifdef WIN32
void normalize_path(std::string& str) {
	string_replace(str, "/", "\\");
}
#else
void normalize_path(std::string& str) {
	return;
}
#endif

// shamelessly copped from
// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
// also this doesn't work lol
bool are_paths_equal(std::string a, std::string b) {
	auto tokenize = [](auto& str) {
		size_t pos = 0;
		std::string delimiter = "\\";
		std::vector<std::string> out;
		while ((pos = str.find(delimiter)) != std::string::npos) {
			out.push_back(str.substr(0, pos));
			str.erase(0, pos + delimiter.length());
		}

		return out;
	};

	auto tokens_a = tokenize(a);
	auto tokens_b = tokenize(b);

	if (tokens_a.size() != tokens_b.size()) return false;

	for (int i = 0; i < tokens_a.size(); i++) {
		if (!are_strings_equal(tokens_a[i], tokens_b[i])) return false;
	}

	return true;
}


// shamelessly copped from
// https://stackoverflow.com/questions/447206/c-isfloat-function
bool is_float(std::string mystring ) {
    std::istringstream iss(mystring);
    float f;
    iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
	
    // Check the entire std::string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail(); 
}

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

#define DEFAULT_FLOAT_TOLERANCE .005
bool float_almost_equals(float a, float b) {
	return glm::abs(a - b) < DEFAULT_FLOAT_TOLERANCE;
}
bool vec_almost_equals(glm::vec2 vec, glm::vec2 target) {
	return glm::length(vec - target) < DEFAULT_FLOAT_TOLERANCE;
}

glm::vec2 vec_divide(glm::vec2 vec, float by) {
	return glm::vec2 { vec.x / by, vec.y / by };
}


// Colors
namespace Colors {
	glm::vec4 Hannah = glm::vec4(.82f, .77f, 0.57f, 1.0f); // Note: Hannah's favorite three floating point numbers.
	glm::vec4 Red = glm::vec4(1.f, 0.f, 0.f, 1.f);
	glm::vec4 Green = glm::vec4(0.f, 1.f, 0.f, 1.f);
	glm::vec4 Blue = glm::vec4(0.f, 0.f, 1.f, 1.f);
	glm::vec4 Brown = glm::vec4(173.f / 255.f, 133.f / 255.f, 74.f / 255.f, 1.f);
	glm::vec4 Black = glm::vec4(0.f, 0.f, 0.f, 1.f);
	glm::vec4 White4 = glm::vec4(1.f, 1.f, 1.f, 1.f);
	glm::vec3 TextWhite = glm::vec3(1.f, 1.f, 1.f);
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
glm::vec3 screen_bottom_right = glm::vec3(1.f, 1.f, 1.f);
glm::vec3 screen_top_right = glm::vec3(1.f, -1.f, 1.f);
glm::vec3 screen_top_left = glm::vec3(-1.f, -1.f, 1.f);
glm::vec3 screen_bottom_left = glm::vec3(-1.f, 1.f, 1.f);

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
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

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
typedef int pixel_unit;
typedef float subpixel_unit;
typedef float screen_unit;
typedef float gl_unit;

float internal_resolution_width = 320;
float internal_resolution_height = 180;

// Screen size definitions
subpixel_unit SCREEN_X = (subpixel_unit)640.f;
subpixel_unit SCREEN_Y = (subpixel_unit)360.f;
subpixel_unit CELL_SIZE = (subpixel_unit)16.f;
screen_unit SCR_TILESIZE_X = (screen_unit)(CELL_SIZE / SCREEN_X);
screen_unit SCR_TILESIZE_Y = (screen_unit)(CELL_SIZE / SCREEN_Y);
gl_unit GLSCR_TILESIZE_X = (gl_unit)(2 * SCR_TILESIZE_X);
gl_unit GLSCR_TILESIZE_Y = (gl_unit)(2 * SCR_TILESIZE_Y);

void use_640_360() {
	SCREEN_X = (subpixel_unit)640.f;
	SCREEN_Y = (subpixel_unit)360.f;
	CELL_SIZE = (subpixel_unit)16.f;
	glfwSetWindowSize(g_window, 640, 360);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}
void use_720p() {
	SCREEN_X = (subpixel_unit)1280.f;
	SCREEN_Y = (subpixel_unit)720.f;
	CELL_SIZE = (subpixel_unit)32.f;
	glfwSetWindowSize(g_window, 1280, 720);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}
void use_1080p() {
	SCREEN_X = (subpixel_unit)1920.f;
	SCREEN_Y = (subpixel_unit)1080.f;
	CELL_SIZE = (subpixel_unit)48.f;
	glfwSetWindowSize(g_window, 1920, 1080);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}
void use_1440p() {
	SCREEN_X = (subpixel_unit)2560.f;
	SCREEN_Y = (subpixel_unit)1440.f;
	CELL_SIZE = (subpixel_unit)64.f;
	glfwSetWindowSize(g_window, 2560, 1440);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}
void use_arbitrary_screen_size(int height, int width) {
	SCREEN_X = (subpixel_unit)width;
	SCREEN_Y = (subpixel_unit)height;
	CELL_SIZE = (subpixel_unit)64.f;
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);	
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
	return px / SCREEN_X;
}
screen_unit screen_y_from_px(pixel_unit px) {
	return px / SCREEN_Y;
}
glm::vec2 screen_from_px(glm::ivec2 px) {
	return glm::vec2(px.x / SCREEN_X, px.y / SCREEN_Y);
}
// Puts it in the center of the grid tile
glm::vec2 screen_from_grid(glm::ivec2 grid_pos) { 
	screen_unit x = (screen_unit)(grid_pos.x * SCR_TILESIZE_X);
	screen_unit y = (screen_unit)(grid_pos.y * SCR_TILESIZE_Y);
	x += (screen_unit)(.5 * SCR_TILESIZE_X);
	y += (screen_unit)(.5 * SCR_TILESIZE_Y);
	return glm::vec2(x, y);
} 

// Converting to pixel units
glm::ivec2 px_coords_from_gl_coords(glm::vec2 gl_coords) {
	float y = (gl_coords.y + 1) / 2;
	float x = (gl_coords.x + 1) / 2;
	return glm::ivec2(floor(x * SCREEN_X), floor(y * SCREEN_Y));
}


/* Some utilities for dealing with files, directories, and paths */
// Takes in a directory or file -- returns everything after the first slash
std::string name_from_full_path(std::string path) {
	std::string asset_name;
	for (int ichar = path.size() - 1; ichar > -1; ichar--) {
		if (path.at(ichar) == '/' || path.at(ichar) == '\\') { break; }
		asset_name.insert(asset_name.begin(), path.at(ichar));
	}
	
	return asset_name;
}

// Accepts a filename, not a path. Returns all the characters before the first period.
std::string strip_extension(std::string filename) {
	std::string stripped;
	for (unsigned int ichar = 0; ichar < filename.size(); ichar++) {
		if (filename.at(ichar) == '.') {
			return stripped;
		}
		stripped.push_back(filename.at(ichar));
	}
	
	return stripped;
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

// Allowing alphanumerics, underscores, and periods
bool is_valid_filename(std::string& str) {
	auto is_numeric = [](char c) -> bool { return c >= '0' && c <= '9'; };
	auto is_alpha = [](char c) -> bool { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); };
	auto is_misc_valid = [](char c) -> bool { return (c == '_') || c == '.'; };
	
	for (unsigned int ichar = 0; ichar < str.size(); ichar++) {
		char c = str.at(ichar);
		if (!(is_numeric(c) || is_alpha(c) || is_misc_valid(c))) {
			return false;
		}
	}
	
	return true;
}

// Don't use a leading slash
std::string absolute_path(std::string dir_from_project_root) {
	// Check that we didn't pass this function an absolute path already
	if (dir_from_project_root.find(root_dir) != std::string::npos) {
		return dir_from_project_root;
	}
	return root_dir + dir_from_project_root;
}

std::string relative_path(std::string absolute) {
	string_replace(absolute, root_dir, "");
	return absolute;
}

std::string path_join(std::vector<std::string> items) {
	std::string path = "";
	for (auto& item : items) {
		path += item + "/";
	}
	
	// Trim trailing slash
	return path.substr(0, path.size() - 1);
}

// Are these good to have?
struct Paths {
	std::string character_texture_path;
	std::string other_texture_path;
	std::string script_path;
};
Paths g_paths;

void init_path_constants() {
	g_paths.character_texture_path = absolute_path(path_join({"textures", "src", "characters"}));
	g_paths.other_texture_path = absolute_path(path_join({"textures", "src", "other"}));
	g_paths.script_path = absolute_path(path_join({"src", "scripts"}));
}

std::string script_path(std::string script) {
	auto path = path_join({ g_paths.script_path, script });
	normalize_path(path);
	return path;
}

struct RelativePath {
	RelativePath(std::string path) {
		normalize_path(path);
		this->path = path;
	}
	
	std::string path;
};
	
struct ScriptPath {
	ScriptPath(std::string raw) {
		normalize_path(raw);
		this->path = raw;
	}
	ScriptPath(RelativePath relative) {
		std::string absolute = script_path(relative.path);
		normalize_path(absolute);
		this->path = absolute;
	}

	std::string path;
};

struct AbsolutePath {
	explicit AbsolutePath(std::string raw) {
		normalize_path(raw);
		this->path = raw;
	}
	AbsolutePath(RelativePath relative) {
		std::string absolute = absolute_path(relative.path);
		normalize_path(absolute);
		this->path = absolute;
	}

	// Script paths are stored absolutely, so we can convert for free
	AbsolutePath(ScriptPath absolute) {
		this->path = absolute.path;
	}

	
	std::string path;
};
	


// @hack I'm sure there are PNG headers I could try parsing, but this works!
bool is_png(std::string& asset_path) {
	if (asset_path.size() < 5) { return false; } // "x.png" is the shortest name
	std::string should_be_png_extension = asset_path.substr(asset_path.size() - 4, 4);
	if (should_be_png_extension.compare(".png")) return false;
	return true;
}

bool is_tds(std::string& path) {
	if (path.size() < 5) { return false; } // "x.tds" is the shortest name
	std::string should_be_tds_extension = path.substr(path.size() - 4, 4);
	if (should_be_tds_extension.compare(".tds")) return false;
	return true;
}

bool is_lua(std::string& path) {
	if (path.size() < 5) { return false; } // "x.tds" is the shortest name
	std::string should_be_tds_extension = path.substr(path.size() - 4, 4);
	if (should_be_tds_extension.compare(".lua")) return false;
	return true;
}

std::string get_default_font_path() {
	// @hack use pathjoin
	return absolute_path("fonts/Inconsolata-Regular.ttf");
}


/* ImGui options */

bool debug_show_aabb = false;
bool debug_show_minkowski = false;
bool show_imgui_demo = false;
bool show_fsm_debugger = false;
bool show_console = false;
bool print_framerate = false;
bool dialogue_mode = false;

bool show_tile_selector = true;
bool show_script_selector = true;
bool show_level_selector = true;
bool show_state_tweaker = true;
bool show_task_editor = false;
bool send_kill_signal = false;

float framerate = 0.f;

const std::string ACTIONS_KEY              = "actions";
const std::string ANIMATIONS_KEY           = "Animations";
const std::string BOUNDING_BOX_KEY         = "bounding_box";
const std::string CENTER_KEY               = "center";
const std::string CH_STATE_KEY             = "character_state";
const std::string COMPONENTS_KEY           = "components";
const std::string CONFIG_KEY               = "config";
const std::string CUTSCENES_KEY            = "cutscenes";
const std::string DEFAULT_ANIMATION_KEY    = "default_animation";
const std::string DIALOGUE_KEY             = "dialogue";
const std::string EDITOR_KEY               = "editor";
const std::string ENTITY_KEY               = "entity";
const std::string ENTITIES_KEY             = "entities";
const std::string EXTENTS_KEY              = "extents";
const std::string FONTS_KEY                = "fonts";
const std::string GAME_STATE_KEY           = "game_state";
const std::string HEALTH_KEY               = "health";
const std::string HEIGHT_KEY               = "height";
const std::string HERO_KEY                 = "boon";
const std::string KIND_KEY                 = "kind";
const std::string LEVEL_KEY                = "level";
const std::string LEVELS_KEY               = "levels";
const std::string MOVES_KEY                = "moves";
const std::string NAME_KEY                 = "name";
const std::string NAMES_KEY                = "names";
const std::string NEXT_STATE_KEY           = "next_state";
const std::string PAN_KEY                  = "pan";
const std::string POS_KEY                  = "pos";
const std::string SCALE_KEY                = "scale";
const std::string SCRIPTS_KEY              = "scripts";
const std::string SPEED_KEY                = "speed";
const std::string STATE_KEY                = "State_Machine";
const std::string TEXT_KEY                 = "text";
const std::string TILES_KEY                = "tiles";
const std::string TRANSITIONS_KEY          = "transitions";
const std::string VARS_KEY                 = "vars";
const std::string WIDTH_KEY                = "width";
const std::string WHICH_KEY                = "which";
const std::string X_KEY                    = "x";
const std::string Y_KEY                    = "y";
const std::string Z_KEY                    = "z";


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


// POOL DECLS
struct pool_entry_info {
	bool available : 1;
};
#define DEFAULT_POOL_SIZE 100000

template<typename Data_Type>
struct pool_handle;

// A templated, fixed size pool of objects which stores them in continuous memory and uses handles to interface with the data elements 
template<typename Data_Type, int num_elements>
struct Pool {
	Data_Type* entries;
	pool_entry_info* info;
	
	void init();
	pool_handle<Data_Type> next_available();
	inline Data_Type* get(pool_handle<Data_Type> handle);
	inline void mark_available(pool_handle<Data_Type> handle);
	inline void mark_unavailable(pool_handle<Data_Type> handle);
};

// POOL DEFNS
template<typename Data_Type, int num_elements>
void Pool<Data_Type, num_elements>::init() {
	entries = (Data_Type*)calloc(sizeof(Data_Type), num_elements);
	fox_for(indx, num_elements) {
		Data_Type* entry = entries + indx;
		new (entry) Data_Type;
	}

	// Allocate some memory for the infos and set 
	info = (pool_entry_info*)calloc(sizeof(pool_entry_info), num_elements);
	memset(info, 1, sizeof(pool_entry_info) * num_elements);
}

// A handle to an element in a pool. Wrapper around an int with convenient operators.
template<typename Data_Type>
struct pool_handle {
	int handlecito = -1;
	Pool<Data_Type, DEFAULT_POOL_SIZE>* pool = nullptr;
	
	// These get the actual entity
	Data_Type* deref() {
		return pool->get(*this);
	}
	Data_Type* operator->() {
		return pool->get(*this);
	}
	Data_Type* operator()() {
		if (!*this) return nullptr;
		return pool->get(*this);
	}
	// This gets the handle (i.e. table index)
	int operator*() {
		return handlecito;
	}
	bool operator==(pool_handle<Data_Type> other) {
		return handlecito == other.handlecito;
	}
	bool operator==(int other) {
		return handlecito == other;
	}
	bool operator!=(pool_handle<Data_Type> other) {
		return !(*this == other);
	}
	bool operator!=(int other) {
		return !(*this == other);
	}
	operator bool() {
		return (handlecito != -1) && (pool != nullptr);
	}
};

//@slow
template<typename Data_Type, int num_elements>
pool_handle<Data_Type> Pool<Data_Type, num_elements>::next_available() {
	for (int handlecito = 0; handlecito < num_elements; handlecito++) {
		pool_entry_info entry = info[handlecito];
		if (entry.available) {
			pool_handle<Data_Type> handle = { handlecito, this };
			mark_unavailable(handle);
			return handle;
		}
	}
	
	tdns_log.write("Ran out of handles!");
	fox_assert(false);
	return { -1, nullptr }; // for the compiler
};

template<typename Data_Type, int num_elements>
inline Data_Type* Pool<Data_Type, num_elements>::get(pool_handle<Data_Type> handle) {
	fox_assert(handle);
	fox_assert(*handle >= 0);
	fox_assert(*handle < num_elements);
	fox_assert(this);
	return entries + *handle;
}

template<typename Data_Type, int num_elements>
inline void Pool<Data_Type, num_elements>::mark_available(pool_handle<Data_Type> handle) {
	info[*handle].available = true;
	memset(entries + *handle, 0, sizeof(Data_Type));
}

template<typename Data_Type, int num_elements>
void Pool<Data_Type, num_elements>::mark_unavailable(pool_handle<Data_Type> handle) {
	info[*handle].available = false;
}


struct Circle_Buffer {
	int* data = nullptr;
	int head = 0;
	int capacity = 0;
	int len = 0;
	
	void push_back(int elem);
	std::optional<int> pop_front();
	void clear();
};

void Circle_Buffer::push_back(int elem) {
	fox_assert(len <= capacity);
	fox_assert(head >= 0 && head < capacity);
	if (len == capacity) { return; }
	data[(head + len) % capacity] = elem;
	len++;
}
std::optional<int> Circle_Buffer::pop_front() {
	fox_assert(len <= capacity);
	fox_assert(head >= 0 && head < capacity);
	if (len) {
		int ret = data[head];
		head = (head + 1) % capacity;
		len--;
		return ret;
	}
	return {};
}
void Circle_Buffer::clear() {
	memset(data, 0, capacity * sizeof(data));
	head = 0;
	len = 0;
}

// two ways you can use a vector:
// 1 (more common): always push to the back of the vector. linearly assing. this means there are no holes
// 2: push to random indices and remove things from it. problem with this is that you have holes in your memory
// so if you have something like [ FULL | FULL | EMPTY | FULL | ...empty rest of array... ], where would you push back?
// is it to the first bucket past the last element (which is kind of what you expect, but leads to holes and then 
// things like for loops don't work with your vector because every so often you hit the damn hold
// or is it the first empty slot? i feel like this is better because there are no holes, your vector works with for loops, 
// but you still have to handle deletes
// just shift everything back on vector delete? sounds pretty good to me. 
#define ARR_INITIAL_CAPACITY 8
template<typename T>
struct tdvec {
	T* data;
	bool* present;
	int count;
	int capacity;
	
	tdvec() {
		data = (T*)calloc(ARR_INITIAL_CAPACITY, sizeof(T));
		present = (bool*)calloc(ARR_INITIAL_CAPACITY, sizeof(bool));
		capacity = ARR_INITIAL_CAPACITY;
		count = 0;
	}
	~tdvec() {
		free(data);
		free(present);
	}
	
	void maybe_grow(int new_size) {
		if (new_size > capacity) {
			data = (T*)realloc(data, sizeof(T) * new_size);
			present = (bool*)realloc(present, sizeof(bool) * new_size);
			fox_assert(data);
			fox_assert(present);
			memset(data + capacity, 0, sizeof(T) * (new_size - capacity));
			memset(present + capacity, 0, sizeof(bool) * (new_size - capacity));
			capacity = new_size;
		}
	}
	
	T* push(T item) {
		if (count == capacity) maybe_grow(capacity * 2);
		data[count] = item;
		present[count] = true;
		
		count++;
		
		return back();
	}
	T* push_at(int i, T item) {
		maybe_grow(i + 1); // i is an index, so we need capacity = i + 1
		data[i] = item;
		return &data[i];
	}
	
	T* elem_at(int i) {
		return &data[i];
	}
	T* back() {
		return elem_at(count - 1);
	}
	
	void erase(int i) {
		if (i > capacity - 1) return;
		
		// Only have to memcpy if we're not erasing from the back
		else if (i != (capacity - 1)) {
			memcpy(data + i, data + i + 1, capacity - i - 1);
			memcpy(present + i, present + i + 1, capacity - i - 1);
		} 
		
		// Always zero out the last element
		data[capacity - 1] = 0;
		present[capacity - 1] = 0;
		count--;
	}
	
};


struct Hasher {
	static int hash(const int key) {
		return key;
	}
} hasher;

#define MAP_INITIAL_CAPACITY 8
template<typename Key, typename Value>
struct tdmap {
	tdvec<tdbyte> present;
	tdvec<Key> keys;
	tdvec<Value> values;
	
	tdmap() {
		keys.maybe_grow(MAP_INITIAL_CAPACITY);
		values.maybe_grow(MAP_INITIAL_CAPACITY);
		present.maybe_grow(MAP_INITIAL_CAPACITY);
	}
	
	void map(Key key, Value val) {
		int index = hasher.hash(key);
		while (*present.elem_at(index)) {
			index = (index + 1) % present.capacity;
		}
		keys.push_at(index, key);
		values.push_at(index, val);
		present.push_at(index, 1);
		
		if (((float)keys.count / (float)keys.capacity) > .6) {
			keys.maybe_grow(2 * keys.capacity);
			values.maybe_grow(2 * values.capacity);
			present.maybe_grow(2 * values.capacity);
		}
	}
};



#include <math.h> // fmodf
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static void ShowExampleAppCustomNodeGraph(bool* opened)
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Example: Custom Node Graph", opened))
	{
		ImGui::End();
		return;
	}
	
	// Dummy
	struct Node
	{
		int     ID;
		char    Name[32];
		ImVec2  Pos, Size;
		float   Value;
		ImVec4  Color;
		int     InputsCount, OutputsCount;
		
		Node(int id, const char* name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count) { 
			ID = id; 
			strncpy(Name, name, 31); 
			Name[31] = 0; 
			Pos = pos; 
			Value = value; 
			Color = color; 
			InputsCount = inputs_count; 
			OutputsCount = outputs_count; }
		
		ImVec2 GetInputSlotPos(int slot_no) const { 
			float x = Pos.x + Size.x * ((float)slot_no + 1) / ((float)InputsCount + 1);
			float y = Pos.y + Size.y;
			return ImVec2(x, y); 
		}
		ImVec2 GetOutputSlotPos(int slot_no) const {
			float x = Pos.x + Size.x * ((float)slot_no + 1) / ((float)InputsCount + 1);
			float y = Pos.y;
			return ImVec2(x, y); }
	};
	struct NodeLink
	{
		int     InputIdx, InputSlot, OutputIdx, OutputSlot;
		
		NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
	};
	
	static ImVector<Node> nodes;
	static ImVector<NodeLink> links;
	static bool inited = false;
	static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	static bool show_grid = true;
	static int node_selected = -1;
	if (!inited)
	{
		nodes.push_back(Node(0, "MainTex", ImVec2(40, 50), 0.5f, ImColor(255, 100, 100), 1, 1));
		nodes.push_back(Node(1, "BumpMap", ImVec2(40, 150), 0.42f, ImColor(200, 100, 200), 1, 1));
		nodes.push_back(Node(2, "Combine", ImVec2(270, 80), 1.0f, ImColor(0, 200, 100), 2, 2));
		links.push_back(NodeLink(0, 0, 2, 0));
		links.push_back(NodeLink(1, 0, 2, 1));
		inited = true;
	}
	
	// Draw a list of nodes on the left side
	bool open_context_menu = false;
	int node_hovered_in_list = -1;
	int node_hovered_in_scene = -1;
	ImGui::BeginChild("node_list", ImVec2(100, 0));
	ImGui::Text("Nodes");
	ImGui::Separator();
	for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
	{
		Node* node = &nodes[node_idx];
		ImGui::PushID(node->ID);
		if (ImGui::Selectable(node->Name, node->ID == node_selected))
			node_selected = node->ID;
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_list = node->ID;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	}
	ImGui::EndChild();
	
	ImGui::SameLine();
	ImGui::BeginGroup();
	
	const float NODE_SLOT_RADIUS = 4.0f;
	const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);
	
	// Create our child canvas
	ImGui::Text("%f, %f", ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
	ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
	ImGui::SameLine(ImGui::GetWindowWidth() - 100);
	ImGui::Checkbox("Show grid", &show_grid);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PushItemWidth(120.0f);
	
	ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	// Display grid
	if (show_grid)
	{
		ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
		float GRID_SZ = 64.0f;
		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
			draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
		for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
			draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
	}
	
	// Display links
	draw_list->ChannelsSplit(2);
	draw_list->ChannelsSetCurrent(0); // Background
	for (int link_idx = 0; link_idx < links.Size; link_idx++)
	{
		NodeLink* link = &links[link_idx];
		Node* node_inp = &nodes[link->InputIdx];
		Node* node_out = &nodes[link->OutputIdx];
		ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->InputSlot);
		ImVec2 p2 = offset + node_out->GetInputSlotPos(link->OutputSlot);
		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
	}
	
	// Display nodes
	for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
	{
		Node* node = &nodes[node_idx];
		ImGui::PushID(node->ID);
		ImVec2 node_rect_min = offset + node->Pos;
		
		// Display node contents first
		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::Text("%s", node->Name);
		ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
		ImGui::ColorEdit3("##color", &node->Color.x);
		ImGui::EndGroup();
		
		// Save the size of what we have emitted and whether any of the widgets are being used
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->Size;
		
		// Display node box
		draw_list->ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->Size);
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_scene = node->ID;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}
		bool node_moving_active = ImGui::IsItemActive();
		if (node_widgets_active || node_moving_active)
			node_selected = node->ID;
		if (node_moving_active && ImGui::IsMouseDragging(0))
			node->Pos = node->Pos + ImGui::GetIO().MouseDelta;
		
		ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? IM_COL32(75, 75, 75, 255) : IM_COL32(60, 60, 60, 255);
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		//draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
		for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		
		ImGui::PopID();
	}
	draw_list->ChannelsMerge();
	
	// Open context menu
	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
	{
		node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		open_context_menu = true;
	}
	if (open_context_menu)
	{
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_list != -1)
			node_selected = node_hovered_in_list;
		if (node_hovered_in_scene != -1)
			node_selected = node_hovered_in_scene;
	}
	
	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node)
		{
			ImGui::Text("Node '%s'", node->Name);
			ImGui::Separator();
			if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
			if (ImGui::MenuItem("Delete", NULL, false, false)) {}
			if (ImGui::MenuItem("Copy", NULL, false, false)) {}
		}
		else
		{
			if (ImGui::MenuItem("Add")) { nodes.push_back(Node(nodes.Size, "New node", scene_pos, 0.5f, ImColor(100, 100, 200), 2, 2)); }
			if (ImGui::MenuItem("Paste", NULL, false, false)) {}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
	
	// Scrolling
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling + ImGui::GetIO().MouseDelta;
	
	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();
	
	ImGui::End();
}

void init_imgui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(g_window, false);
	auto& imgui = ImGui::GetIO();
	imgui.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
}

std::string capitalize_component_name(std::string name_from_path) {
	// Always capitalize the first character
	name_from_path[0] = toupper(name_from_path[0]);
	
	// Also capitalize everything after an underscore
	bool capitalize_next_char = false;
	for (auto& c : name_from_path) {
		if (c == '_') {
			capitalize_next_char = true;
			continue;
		}

		if (capitalize_next_char) {
			c -= 32; // Would like to use toupper here...
			capitalize_next_char = false;
		}

	}

	return name_from_path;
}

std::vector<std::string> all_component_types() {
	std::vector<std::string> ignore = {
        "component_includes",
		"component_impl_includes"
	};
	std::string impl = "_impl";

	std::vector<std::string> names;
	
	auto component_dir = path_join({root_dir, "src", "components"});
	for (const auto& entry : directory_iterator(component_dir)) {
		std::string filename = entry.path().filename().replace_extension().string();
		if (tdns_find(ignore, filename)) continue;
		if (does_string_contain_substr(filename, impl)) continue;
		
		auto component_name = capitalize_component_name(filename);
		component_name += "_Component";
		names.push_back(component_name);
	}
	
	return names;
}

std::vector<std::string> tile_size_descriptions = {
	"-1.5 tiles",
	"-1.25 tiles",
	"-1 tiles",
	"-.5 tiles",
	"0 tiles",
	".5 tiles",
	"1 tile",
	"1.25 tiles",
	"1.5 tiles",
	"2 tiles",
	"3 tiles",
	"4 tiles",
	"5 tiles",
	"6 tiles",
};

std::vector<double> tile_sizes = {
	-0.0666,
	-0.0555,
	-0.0444,
	-0.0222,
	0, 
	0.0222,
	0.0444,
	0.0555,
	0.0666,
	0.0888,
	0.1333,
	0.1777,
	0.2222,
	0.2666,
};

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
struct FileWatcher {
	std::map<std::string, std::filesystem::file_time_type> time_map;
	std::map<std::string, FileChangedCallback> action_map;

	void watch(AbsolutePath file, FileChangedCallback on_change) {
		time_map[file.path] = std::filesystem::last_write_time(file.path);
		action_map[file.path] = on_change;
	}

	void update() {
		for (auto& [path, last_known_write_time] : time_map) {
			auto last_write_time = std::filesystem::last_write_time(path);
			if (last_known_write_time < last_write_time) {
				time_map[path] = last_write_time;
				action_map[path]();
			}
		}
	}
	
};
FileWatcher file_watcher;
