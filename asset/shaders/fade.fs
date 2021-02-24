#version 330 core
out vec4 fragment_color;

in vec2 frag_tex_coord;

uniform sampler2D screen;

void main()
{
	vec4 screen_color = texture(screen, frag_tex_coord);
	vec4 mix_color = vec2(1.0, 0.0, 0.0, 1.0);
	fragment_color = mix(screen_color, mix_color, .5);
}
