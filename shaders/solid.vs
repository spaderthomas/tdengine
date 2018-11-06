#version 330 core
layout (location = 0) in vec2 pos;

uniform mat3 transform;

void main() {
	gl_Position = vec4(transform * vec3(pos, 1.f), 1.f);
}	