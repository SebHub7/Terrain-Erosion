#version 330

#ifdef VERTEX_SHADER
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 translations;

uniform mat4 vpMatrix;
out vec3 n;
out vec2 uv2;

void main()
{
    n = normal;
    uv2 = uv;
    gl_Position = vpMatrix * vec4(position + translations, 1);
}

#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;
in vec3 n;
in vec2 uv2;
vec3 light = normalize(vec3(-1, 1, 0));

uniform sampler2D fir_text;

void main()
{
    float cos_theta = dot(light, n);
    vec3 pixel = texture(fir_text, uv2).rgb;
    fragment_color = vec4(pixel * cos_theta, 1);
}

#endif
