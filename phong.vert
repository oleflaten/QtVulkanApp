#version 450

layout(location = 0) in vec3 vertexPosition;    //renamed from earlier shaders
layout(location = 1) in vec3 vertexNormal;      //renamed from earlier shaders
layout(location = 2) in vec2 vertexUV;          //not used when we don't use textures

layout(location = 0) out vec3 fragmentPosition;
layout(location = 1) out vec3 normalTransposed;
layout(location = 2) out vec2 UV;               //for textures
layout(location = 3) out vec3 objectColor;

layout(push_constant) uniform mod {
    mat4 mMatrix;
    vec3 objectColor;
} uModel;

layout(std140, binding = 0) uniform buf {
   mat4 vMatrix;
   mat4 pMatrix;
} uBuffer;

void main()
{
   fragmentPosition = vec3(uModel.mMatrix * vec4(vertexPosition, 1.0));    //1.0 because it is a point
   normalTransposed = mat3(transpose(inverse(uModel.mMatrix))) * vertexNormal;

   UV = vertexUV;      //for textures
   objectColor = uModel.objectColor;
   gl_Position = uBuffer.pMatrix * uBuffer.vMatrix * uModel.mMatrix * vec4(vertexPosition, 1.0);
}

//Using calculations in world space,
//https://learnopengl.com/Lighting/Basic-Lighting
//but could just as easy be done in camera space
//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/
