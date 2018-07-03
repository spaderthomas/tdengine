#version 330 core
layout (location = 0) in vec4 vertex;

out vec2 frag_tex_coord;

uniform mat3 transform;
uniform float z;

void main() {
    vec3 pos = transform * vec3(vertex.xy, 1.0);
    gl_Position = vec4(pos.x, pos.y, z, 1.0);
	frag_tex_coord = vertex.zw;
}