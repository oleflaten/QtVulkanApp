#version 440

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 v_color;

layout(push_constant) uniform buf {
    mat4 mvp;
    vec3 objectcolor;
} ubuffer;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    //if objectcolor is not set (== black), use vertex color
    float test = ubuffer.objectcolor.r + ubuffer.objectcolor.g +ubuffer.objectcolor.b;
    if (test < 0.001)
        v_color = color;
    //else use objectcolor
    else
        v_color = ubuffer.objectcolor;

    gl_Position = ubuffer.mvp * position;
}
