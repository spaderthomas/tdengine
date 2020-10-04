#version 330 core
out vec4 frag_color;

in vec2 frag_tex_coord;

uniform sampler2D sampler;
uniform vec3 text_color;

void main()
{
    // Textures are stored in just the red component -- really just a grayscale value
    vec4 sampled = vec4(1.f, 1.f, 1.f, texture(sampler, frag_tex_coord).r);

    // Scale our color by the grayness
    frag_color = vec4(text_color, 1.f) * sampled;
} 