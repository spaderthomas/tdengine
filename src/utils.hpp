#define fox_for(iterName, iterCount) for (unsigned int iterName = 0; iterName < (iterCount); ++iterName)
typedef unsigned int uint;
typedef int32_t int32;

template<typename vec_type>
void concat(vector<vec_type>& append_to, vector<vec_type>& append_from) {
	append_to.insert(append_to.end(), append_from.begin(), append_from.end());
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
GLuint last_bound_texture;

float seconds_per_update = 1.f / 60.f;

// Default to half of 720p
float SCREEN_X = 640.f;
float SCREEN_Y = 360.f;
float CELL_SIZE = 16.f;
float SCR_TILESIZE_X = CELL_SIZE / SCREEN_X;
float SCR_TILESIZE_Y = CELL_SIZE / SCREEN_Y;
float GLSCR_TILESIZE_X = 2 * SCR_TILESIZE_X;
float GLSCR_TILESIZE_Y = 2 * SCR_TILESIZE_Y;

void use_640_360(GLFWwindow* window) {
	SCREEN_X = 640.f;                     
	SCREEN_Y = 360.f;                     
	CELL_SIZE = 16.f;                     
	SCR_TILESIZE_X = CELL_SIZE / SCREEN_X;
	SCR_TILESIZE_Y = CELL_SIZE / SCREEN_Y;
	GLSCR_TILESIZE_X = 2 * SCR_TILESIZE_X;
	GLSCR_TILESIZE_Y = 2 * SCR_TILESIZE_Y;
	glfwSetWindowSize(window, 640, 360);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}
void use_720p(GLFWwindow* window) {
	SCREEN_X = 1280.f;                     
	SCREEN_Y = 720.f;                     
	CELL_SIZE = 32.f;                     
	SCR_TILESIZE_X = CELL_SIZE / SCREEN_X;
	SCR_TILESIZE_Y = CELL_SIZE / SCREEN_Y;
	GLSCR_TILESIZE_X = 2 * SCR_TILESIZE_X;
	GLSCR_TILESIZE_Y = 2 * SCR_TILESIZE_Y;
	glfwSetWindowSize(window, 1280, 720);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}
void use_1080p(GLFWwindow* window) {
	SCREEN_X = 1920.f;                     
	SCREEN_Y = 1080.f;                     
	CELL_SIZE = 64.f;                     
	SCR_TILESIZE_X = CELL_SIZE / SCREEN_X;
	SCR_TILESIZE_Y = CELL_SIZE / SCREEN_Y;
	GLSCR_TILESIZE_X = 2 * SCR_TILESIZE_X;
	GLSCR_TILESIZE_Y = 2 * SCR_TILESIZE_Y;
	glfwSetWindowSize(window, 1920, 1080);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}


// Colors
// Note: Hannah's favorite three floating point numbers. Do not remove!
glm::vec4 hannah_color = glm::vec4(.82f, .77f, 0.57f, 1.0f);
glm::vec4 red = glm::vec4(1.f, 0.f, 0.f, 1.f);
glm::vec4 green = glm::vec4(0.f, 1.f, 0.f, 1.f);
glm::vec4 blue = glm::vec4(0.f, 0.f, 1.f, 1.f);


// This defines which tile is on the upper left of the screen
glm::ivec2 camera_top_left = glm::ivec2(0);


// Functions to convert between units
glm::ivec2 grid_pos_from_px_pos(glm::vec2 px_pos) {
	int closest_x = (int)floor(px_pos.x / CELL_SIZE);
	int closest_y = (int)floor(px_pos.y / CELL_SIZE);
	return glm::ivec2(closest_x, closest_y);
}
glm::vec2 gl_coords_from_screen_coords(glm::vec2 screen_coords) {
	return glm::vec2(screen_coords.x * 2 - 1, 1 - screen_coords.y * 2);
}

// Takes in a directory or file -- returns everything after the first double backslash
string name_from_full_path(string path) {
	string asset_name;
	for (int ichar = path.size() - 1; ichar > -1; ichar--) {
		if (path.at(ichar) == '\\') { break; }
		if (path.at(ichar) == '/') {
			string msg = "Don't use forward slashes in your directory names. Failing path was: " + path;
			tdns_log.write(msg);
			exit(0);
		}
		asset_name.insert(asset_name.begin(), path.at(ichar));
	}

	return asset_name;
}

// Accepts a filename, not a path. Returns all the characters before the first period.
string strip_extension(string filename) {
	string stripped;
	for (unsigned int ichar = 0; ichar < filename.size(); ichar++) {
		if (filename.at(ichar) == '.') {
			return stripped;
		}
		stripped.push_back(filename.at(ichar));
	}

	return stripped;
}

bool is_alphanumeric(string& str) {
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
bool is_valid_texture_name(string& str) {
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

// @hack I'm sure there are PNG headers I could try parsing, but this works!
bool is_png(string& asset_path) {
	if (asset_path.size() < 5) { return false; } // "x.png" is the shortest name
	string should_be_png_extension = asset_path.substr(asset_path.size() - 4, 4);
	if (should_be_png_extension.compare(".png")) return false;
	return true;
}

#define fox_max(a, b) (a) > (b) ? (a) : (b)
#define fox_min(a, b) (a) > (b) ? (b) : (a)