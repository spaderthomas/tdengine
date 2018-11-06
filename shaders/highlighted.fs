#version 330 core
out vec4 frag_color;

in vec2 frag_tex_coord;

uniform sampler2D sampler;

void main()
{
    frag_color = texture(sampler, frag_tex_coord);
    float boosted_red = frag_color.x * 2;
    frag_color.x = boosted_red - (256 * floor(boosted_red / 256));
} 