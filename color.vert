#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 UV;

layout(location = 0) out vec3 vColor;

layout(push_constant) uniform mod {
    mat4 model;
    vec3 objectColor;
} uModel;

layout(set = 0, binding = 0) uniform cam {
    mat4 view;
    mat4 projection;
} uBuffer;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    //if objectcolor is not set (== black), use vertex color
    float colorTest = uModel.objectColor.r + uModel.objectColor.g +uModel.objectColor.b;
    if (colorTest < 0.001)
        vColor = color;
    //else use objectcolor
    else
        vColor = uModel.objectColor;
    gl_Position =   uBuffer.projection * uBuffer.view * uModel.model * vec4(position, 1.0);
}

