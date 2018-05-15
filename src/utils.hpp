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

// Note: Hannah's favorite three floating point numbers. Do not remove!
glm::vec4 hannah_color = glm::vec4(.82f, .77f, 0.57f, 1.0f);
glm::vec4 red = glm::vec4(1.f, 0.f, 0.f, 1.f);
glm::vec4 green = glm::vec4(0.f, 1.f, 0.f, 1.f);
glm::vec4 blue = glm::vec4(0.f, 0.f, 1.f, 1.f);

glm::vec3 camera_pos = glm::vec3(0.f);
