#version 330 core
out vec4 frag_color;

in vec2 frag_tex_coord;
in float mix;

uniform sampler2D sampler1;

void main()
{
    frag_color = texture(sampler1, frag_tex_coord);
    //frag_color = texture(sampler2, frag_tex_coord);
} 