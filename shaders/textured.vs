#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 frag_tex_coord;

uniform vec2 camera_pos;
uniform mat3 transform;
uniform float z;

void main() {
    vec3 xyz_pos = vec3(pos, 1.f);
	vec3 world_pos = transform * xyz_pos;
	world_pos = vec3(world_pos.x, world_pos.y, z);
	vec3 screen_pos = world_pos - vec3(camera_pos, 0.f);
	gl_Position = vec4(screen_pos, 1.f);
	
	frag_tex_coord = tex_coord;
}