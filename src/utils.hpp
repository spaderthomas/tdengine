 // Small quality of life macros and typedefs
 #define fox_max(a, b) (a) > (b) ? (a) : (b)
 #define fox_min(a, b) (a) > (b) ? (b) : (a)
 #define fox_for(iterName, iterCount) for (unsigned int iterName = 0; iterName < (iterCount); ++iterName)
 #define fox_iter(iter_name, container) for (auto iter_name = (container).begin(); (iter_name) != (container).end(); (iter_name)++)
 #define rand_float(max) (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max)))
 #define is_newline(c) ((c) == '\n' || (c) == '\r')
 #define is_space(c) ((c) == ' ')

 //Assert
 #ifdef _MSC_VER
 #	ifdef assert
 #		undef assert
 #	endif
 #	define fox_assert(expr) if (!(expr)) _CrtDbgBreak()
 #else
 #	define fox_assert(expr) assert(expr)
 #endif
 typedef unsigned int uint;
 typedef int32_t int32;
typedef unsigned char tdbyte;

 // STL extensions 
 template<typename vec_type>
 void concat(vector<vec_type>& append_to, vector<vec_type>& append_from) {
 	append_to.insert(append_to.end(), append_from.begin(), append_from.end());
 }

 vector<string> split(const string &str, char delim) {
 	stringstream stream(str);
 	string item;
 	vector<string> tokens;
 	while (getline(stream, item, delim)) {
 		tokens.push_back(item);
 	}
 	return tokens;
 }

 #define tdns_find(vector, item) (find((vector).begin(), (vector).end(), (item)) != (vector).end()) 

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
 // Colors
 glm::vec4 hannah_color = glm::vec4(.82f, .77f, 0.57f, 1.0f); // Note: Hannah's favorite three floating point numbers.
 glm::vec4 red = glm::vec4(1.f, 0.f, 0.f, 1.f);
 glm::vec4 green = glm::vec4(0.f, 1.f, 0.f, 1.f);
 glm::vec4 blue = glm::vec4(0.f, 0.f, 1.f, 1.f);
 glm::vec4 brown = glm::vec4(173.f / 255.f, 133.f / 255.f, 74.f / 255.f, 1.f);
 glm::vec4 black = glm::vec4(0.f, 0.f, 0.f, 1.f);
 glm::vec4 white4 = glm::vec4(1.f, 1.f, 1.f, 1.f);
 glm::vec3 white3 = glm::vec3(1.f, 1.f, 1.f);




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

 // Screen size definitions
 subpixel_unit SCREEN_X = (subpixel_unit)640.f;
 subpixel_unit SCREEN_Y = (subpixel_unit)360.f;
 subpixel_unit CELL_SIZE = (subpixel_unit)16.f;
 screen_unit SCR_TILESIZE_X = (screen_unit)(CELL_SIZE / SCREEN_X);
 screen_unit SCR_TILESIZE_Y = (screen_unit)(CELL_SIZE / SCREEN_Y);
 gl_unit GLSCR_TILESIZE_X = (gl_unit)(2 * SCR_TILESIZE_X);
 gl_unit GLSCR_TILESIZE_Y = (gl_unit)(2 * SCR_TILESIZE_Y);

 void use_640_360(GLFWwindow* window) {
 	SCREEN_X = (subpixel_unit)640.f;
 	SCREEN_Y = (subpixel_unit)360.f;
 	CELL_SIZE = (subpixel_unit)16.f;
 	SCR_TILESIZE_X = (screen_unit)(CELL_SIZE / SCREEN_X);
 	SCR_TILESIZE_Y = (screen_unit)(CELL_SIZE / SCREEN_Y);
 	GLSCR_TILESIZE_X = (gl_unit)(2 * SCR_TILESIZE_X);
 	GLSCR_TILESIZE_Y = (gl_unit)(2 * SCR_TILESIZE_Y);
 	glfwSetWindowSize(window, 640, 360);
 	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
 }
 void use_720p(GLFWwindow* window) {
 	SCREEN_X = (subpixel_unit)1280.f;
 	SCREEN_Y = (subpixel_unit)720.f;
 	CELL_SIZE = (subpixel_unit)32.f;
 	SCR_TILESIZE_X = (screen_unit)(CELL_SIZE / SCREEN_X);
 	SCR_TILESIZE_Y = (screen_unit)(CELL_SIZE / SCREEN_Y);
 	GLSCR_TILESIZE_X = (gl_unit)(2 * SCR_TILESIZE_X);
 	GLSCR_TILESIZE_Y = (gl_unit)(2 * SCR_TILESIZE_Y);
 	glfwSetWindowSize(window, 1280, 720);
 	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
 }
 void use_1080p(GLFWwindow* window) {
 	SCREEN_X = (subpixel_unit)1920.f;
 	SCREEN_Y = (subpixel_unit)1080.f;
 	CELL_SIZE = (subpixel_unit)48.f;
 	SCR_TILESIZE_X = (screen_unit)(CELL_SIZE / SCREEN_X);
 	SCR_TILESIZE_Y = (screen_unit)(CELL_SIZE / SCREEN_Y);
 	GLSCR_TILESIZE_X = (gl_unit)(2 * SCR_TILESIZE_X);
 	GLSCR_TILESIZE_Y = (gl_unit)(2 * SCR_TILESIZE_Y);
 	glfwSetWindowSize(window, 1920, 1080);
 	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
 }
 // Converting to grid units
 glm::ivec2 grid_pos_from_px_pos(glm::vec2 px_pos) {
 	int closest_x = (int)floor(px_pos.x / CELL_SIZE);
 	int closest_y = (int)floor(px_pos.y / CELL_SIZE);
 	return glm::ivec2(closest_x, closest_y);
 }
 glm::ivec2 grid_from_world(glm::vec2 world_pos) {
 	int closest_x = (int)floor(world_pos.x / SCR_TILESIZE_X);
 	int closest_y = (int)floor(world_pos.y / SCR_TILESIZE_Y);
 	return glm::ivec2(closest_x, closest_y);
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
 bool is_valid_filename(string& str) {
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
string absolute_path(string dir_from_project_root) {
	return root_dir + dir_from_project_root;
}

// @hack I'm sure there are PNG headers I could try parsing, but this works!
bool is_png(string& asset_path) {
	if (asset_path.size() < 5) { return false; } // "x.png" is the shortest name
	string should_be_png_extension = asset_path.substr(asset_path.size() - 4, 4);
	if (should_be_png_extension.compare(".png")) return false;
	return true;
}


/* ImGui options */
bool show_grid = false;
bool snap_to_grid = false;
bool debug_show_aabb = false;
bool debug_show_minkowski = false;
bool show_imgui_demo = false;
bool show_fsm_debugger = false;
bool show_console = false;
bool print_framerate = false;
bool dialogue_mode = false;

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

	string debug_msg;
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
	debug_msg += to_string(id);

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

	debug_msg += "\nGL message: " + string(message);
	debug_msg += "\n\n";
	tdns_log.write(debug_msg);
}

// This defines which tile is on the upper left of the screen
float seconds_per_update = 1.f / 60.f;
double pi = 3.14159;

void fixup_type_name(string& type_name) {
	vector<string> extraneous = { " *", " const", "struct " };
	for (auto& str : extraneous) {
		int erase_pos = type_name.find(str);
		if (erase_pos >= 0) {
			type_name.erase(erase_pos, str.length());
		}
	}
}

glm::vec2 vec2_max(glm::vec2 a, glm::vec2 b) {
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


// POOL DECLS
struct pool_entry_info {
	bool available : 1;
};
#define DEFAULT_POOL_SIZE 10000

/* fuck cpp */
template<typename Data_Type>
struct pool_handle;
/* fuck cpp */

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
	info = (pool_entry_info*)calloc(sizeof(pool_entry_info), num_elements);
	memset(info, 1, sizeof(pool_entry_info) * num_elements);
}

// A handle to an element in a pool. Wrapper around an int with convenient operators.
template<typename Data_Type>
struct pool_handle {
	int handlecito;
	Pool<Data_Type, DEFAULT_POOL_SIZE>* pool;

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
	optional<int> pop_front();
	void clear();
};

struct Key_Type {
	int x;
	int y;
};

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

struct Hasher {
	static int hash(const Key_Type& key) {
		return key.x + key.y;
	}

	static int hash(const int key) {
		return key;
	}

	static int hash(const type_info* key) {
		return (int)key & 0x00000001;
	}
} hasher;


