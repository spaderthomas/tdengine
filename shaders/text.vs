#version 330 core
layout (location = 0) in vec4 vertex;

out vec2 frag_tex_coord;

uniform mat4 projection;

void main() {
    vec4 pos = projection * vec4(vertex.xy, 0.0, 1.0);
    gl_Position = vec4(pos.x, pos.y, 1.0, 1.0);
	frag_tex_coord = vertex.zw;
}