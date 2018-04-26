#define fox_for(iterName, iterCount) for (unsigned int iterName = 0; iterName < (iterCount); ++iterName)
typedef unsigned int uint;

template<typename vec_type>
void concat(vector<vec_type>& append_to, vector<vec_type>& append_from) {
	append_to.insert(append_to.end(), append_from.begin(), append_from.end());
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void gl_unbind_buffer() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void gl_bind_buffer(GLint buf) {
	glBindBuffer(GL_ARRAY_BUFFER, buf);
}
