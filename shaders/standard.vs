#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 fs_tex_coord;

uniform mat3 transform;

void main() {
	gl_Position = vec4(transform * pos, 1.f);
	
	fs_tex_coord = tex_coord;
}