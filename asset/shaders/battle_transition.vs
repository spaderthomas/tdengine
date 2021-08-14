#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coord;

out vec2 fs_tex_coord;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	fs_tex_coord = tex_coord;
}