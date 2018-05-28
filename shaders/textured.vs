#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 frag_tex_coord;

uniform mat3 transform;
uniform vec3 camera_translation;
uniform vec3 test;

void main() {
    vec3 xyz_pos = vec3(pos, 0.f);
	xyz_pos = xyz_pos + test;
	xyz_pos = xyz_pos - test;
	vec3 world_pos = transform * xyz_pos;
	vec3 screen_pos = world_pos + camera_translation;
	gl_Position = vec4(screen_pos, 1.f);
	
	frag_tex_coord = tex_coord;
}