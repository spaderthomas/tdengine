#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coord;

out vec2 frag_tex_coord;
out float mix;

void main() {
	float dist = distance(pos, vec3(vec2(1.f), 0.f));
	mix = dist * dist / 4.f;
    gl_Position = vec4(pos, 1.f);
	frag_tex_coord = tex_coord;
}