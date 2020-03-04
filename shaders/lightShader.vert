#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 eye;

// Matrices used to transform normal variables
uniform mat4 projectionTI;
uniform mat4 viewTI;
uniform mat4 modelTI;

// Information from the material
uniform vec3 color;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

// Information from the light, dir and position, one for directional light and one for point light
uniform vec3 lightDir;
uniform vec3 lightPosition;
uniform mat3 dirLightColor;
uniform mat3 pointLightColor;

// Control gate -- which light source to consider
uniform bool dirLightOn;
uniform bool pointLightOn;
uniform bool virtualObject;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out vec3  colorInfo;
out float alphaValue;

void main()
{
    alphaValue  = 1.0f;

    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    vec4 vertexPosition4 = model * vec4(position, 1.0);
    gl_Position = projection * view * vertexPosition4;
    
    vec4 normal4     = modelTI * vec4(normal, 1.0);
    vec3 normalNor   = normalize(vec3(normal4[0], normal4[1], normal4[2]));
    
    vec3 colorResult = vec3(0.0, 0.0, 0.0);
    
    if (virtualObject){
        colorInfo = color;
        return;
    }

    if (dirLightOn) {
        vec3 vertexPosition = vec3(vertexPosition4[0], vertexPosition4[1], vertexPosition4[2]);
        
        // Calculate color with light
        vec3 lightDirNor = normalize(lightDir);
        float diffuseCo = max(dot(normalNor, lightDirNor), 0.0);
    
        // Specular: -gl_Position is the viewing directory (?)
        vec3 reflectDir = normalize(reflect(lightDirNor, normalNor));
        vec3 viewDir = normalize(vertexPosition-eye);
        float specularCo = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
        vec3 dirResult = dirLightColor * ambient + diffuseCo * dirLightColor * diffuse + specularCo * dirLightColor * specular;
        if (dirResult[0] >=0 && dirResult[1] >=0 && dirResult[2] >= 0) colorResult += dirResult;
    }
    
    if (pointLightOn){
        vec3 vertexPosition = vec3(vertexPosition4[0], vertexPosition4[1], vertexPosition4[2]);
        vec3 lightDir = normalize(lightPosition - vertexPosition);
        
        // diffuse shading
        float diffuseCo = max(dot(normalNor, lightDir), 0.0);
        vec3 diffuseRes = diffuseCo * pointLightColor * diffuse;
    
        // specular
        vec3 reflectDir = normalize(reflect(-lightDir, normalNor));
        float specCo = pow(max(dot(normalize(eye-vertexPosition), 
            reflectDir), 0.0), shininess);
        vec3 specularRes = specCo * pointLightColor * specular;
    
        // ambient
        vec3 ambientRes  = pointLightColor * ambient;
    
        // attenuation
        float dist = length(lightPosition - vertexPosition);
        // Constants to control linear attenuation
        float a, b;
        a = 1;
        b = 1;
        float attenuation = 1.0 / (a + b * dist);
    
        vec3 pointResult = ambientRes * attenuation + diffuseRes * attenuation + specularRes * attenuation;
        if (pointResult[0] >=0 && pointResult[1] >=0 && pointResult[2] >= 0) colorResult += pointResult;
    }

    colorInfo = colorResult;
}