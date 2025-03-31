#version 440

layout(location = 0) out vec4 fragColor;         //The final color

layout(location = 0) in vec3 normalTransposed;   //Normal of fragment, already transposed
layout(location = 1) in vec3 fragmentPosition;
layout(location = 2) in vec2 UV;       //for textures
layout(location = 3) in vec3 objectColor;

//uniform sampler2D textureSampler;

layout(std140, binding = 1) uniform buf {
    vec3 lightPosition;
    vec3 lightColor;
    vec3 cameraPosition;
    float ambientStrengt;
    float lightStrengt;
    float specularStrength;

    int specularExponent;
} uBuffer;

void main() {
    //ambient
    vec3 ambient = uBuffer.ambientStrengt * uBuffer.lightColor;

    //diffuse
    vec3 normalCorrected = normalize(normalTransposed);
    vec3 lightDirection = normalize(uBuffer.lightPosition - fragmentPosition);
    float angleFactor = max(dot(normalCorrected, lightDirection), 0.0);
    vec3 diffuse = angleFactor * objectColor * uBuffer.lightColor * uBuffer.lightStrengt;

    //specular
    vec3 viewDirection = normalize(uBuffer.cameraPosition - fragmentPosition);
    float spec = 0.0;
    if (angleFactor > 0.0)     //calculations only needed if diffuse is above 0.0
    {
        vec3 reflectDirection = reflect(-lightDirection, normalCorrected);
        spec = pow(max(dot(viewDirection, reflectDirection), 0.0), uBuffer.specularExponent);
    }
    vec3 specular = spec * uBuffer.lightColor * uBuffer.specularStrength;

    vec3 result = ambient + diffuse + specular;

    fragColor = vec4(result, 1.0) * vec4(objectColor, 1.0);    //texture(textureSampler, UV
    //fragColor =  uBuffer.lightStrengt * vec4(objectColor, 1.0);    //texture(textureSampler, UV);
}

//Using calculations in world space,
//https://learnopengl.com/Lighting/Basic-Lighting
//but could just as easy be done in camera space
//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/
