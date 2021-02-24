#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 frag_tex_coord;

uniform mat3 transform;

void main() {
    vec3 pos_three = vec3(pos, 1.f);
	gl_Position = vec4(transform * pos_three, 1.f);

	frag_tex_coord = tex_coord;
}
