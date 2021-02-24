#version 330 core
out vec4 fragment_color;

in vec2 tex_coords;

uniform sampler2D screen;

void main()
{
	fragment_color = texture(screen, tex_coords)
}
