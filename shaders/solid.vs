#version 330 core
layout (location = 0) in vec3 pos;

uniform mat3 transform;

void main() {
    vec3 xy_pos = vec3(vec2(pos), 1.f);
	gl_Position = vec4(vec2(transform * xy_pos), pos.z, 1.f);
}