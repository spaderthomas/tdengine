#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 frag_tex_coord;

uniform mat3 transform;
uniform int z;

void main() {
    vec3 xyz_pos = vec3(pos, 1.f);
	vec3 world_pos = transform * xyz_pos;
	world_pos = vec3(world_pos.x, world_pos.y, z);
	gl_Position = vec4(world_pos, 1.f);
	
	frag_tex_coord = tex_coord;
}