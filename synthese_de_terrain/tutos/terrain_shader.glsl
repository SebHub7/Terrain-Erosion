#version 330

#ifdef VERTEX_SHADER
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 vpMatrix;
out vec3 n;
out vec2 uv2;

void main()
{
    n = normal;
    uv2 = uv;
    gl_Position = vpMatrix * vec4(position, 1);
}

#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;
in vec3 n;
in vec2 uv2;
vec3 light = normalize(vec3(1, 1, 0));

uniform sampler2D text;

void main()
{
    float cos_theta = dot(light, n);
    vec3 pixel = texture(text, uv2).rgb;
    fragment_color = vec4(pixel * cos_theta, 1);
    //fragment_color = vec4(vec3(cos_theta), 1);
    //fragment_color = vec4(n, 1);
}

#endif
