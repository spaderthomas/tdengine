#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 frag_tex_coord;

uniform mat3 transform;
uniform vec3 camera_translation;

void main() {
    vec3 xy_pos = vec3(vec2(pos), 1.f);
	vec3 world_pos = vec3(vec2(transform * xy_pos), pos.z);
	vec3 screen_pos = world_pos + camera_translation;
	gl_Position = vec4(screen_pos, 1.f);
	
	frag_tex_coord = tex_coord;
}