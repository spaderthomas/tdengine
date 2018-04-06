#version 330 core
layout (location = 0) in vec3 aPos;

uniform float x_offset;

void main() {
    gl_Position = vec4(aPos.x + x_offset, aPos.y, aPos.z, 1.0);
}