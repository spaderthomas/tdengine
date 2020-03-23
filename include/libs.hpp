// Library includes
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_rect_pack.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H  
}

#include "lua_includes.hpp"

#include "sol.hpp"

#include "imgui/imgui.h"
#include "imgui_impl_glfw_gl3.hpp"
