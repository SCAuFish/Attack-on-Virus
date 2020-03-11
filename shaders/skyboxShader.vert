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

uniform int drawSkybox;
uniform float time = 0.0;

out vec3 colorInfo;
out vec3 TexCoords;
out vec3 pos;
out vec3 fsun;

void main()
{

    
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * view * model * vec4(position, 1.0);
    // divide by size of the skybox
    pos = transpose(mat3(view)) * (inverse(projection) * gl_Position / 2048).xyz;
    fsun = vec3(0.0, sin(time * 0.01), cos(time * 0.01));
    TexCoords   = position;
    
    colorInfo   = 0.5*normalize(normal)+0.5;

    // Correct if painting separately
//    if (drawSkybox == 2){
//        // drawing cloud
//        gl_Position = vec4(data[gl_VertexID], 0.0, 1.0);
//        pos = transpose(mat3(view)) * (inverse(projection) * gl_Position).xyz;
//        fsun = vec3(0.0, sin(time * 0.01), cos(time * 0.01));
//    } else {
//        // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
//        gl_Position = projection * view * model * vec4(position, 1.0);
//        TexCoords   = position;
//        
//        colorInfo   = 0.5*normalize(normal)+0.5;
//    }
}