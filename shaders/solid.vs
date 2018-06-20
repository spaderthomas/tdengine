#version 330 core
layout (location = 0) in vec3 pos;

uniform mat3 transform;

void main() {
	gl_Position = vec4(transform * pos, 1.f);
}