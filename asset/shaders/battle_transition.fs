#version 330 core
out vec4 fragment_color;

in vec2 fs_tex_coord;

uniform sampler2D screen;
uniform sampler2D effect;
uniform float cutoff;

void main() {
    vec4 sample_from_effect = texture(effect, fs_tex_coord);
	if (sample_from_effect.r < cutoff) {
	    fragment_color = vec4(0, 0, 0, 1);
	} else {
	    fragment_color = texture(screen, fs_tex_coord);
	}
}
