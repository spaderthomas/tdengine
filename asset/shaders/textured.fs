#version 330 core
out vec4 frag_color;

in vec2 frag_tex_coord;

uniform sampler2D sampler;

void main()
{
    frag_color = texture(sampler, frag_tex_coord);
}

