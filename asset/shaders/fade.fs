#version 330 core
out vec4 fragment_color;

in vec2 fs_tex_coord;

uniform sampler2D screen;
uniform float time;

void main() {
	vec4 screen_color = texture(screen, fs_tex_coord);
	vec4 mix_color = vec4(0.0, 0.0, 0.0, 1.0);
	fragment_color = mix(mix_color, screen_color, time);
}
