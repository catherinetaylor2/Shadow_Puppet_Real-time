#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 UVcoords;

uniform mat4 depthMVP; 
uniform mat4 rotation;

out vec2 UV;
out float Z;

void main(){
    mat4 T = mat4(1.0f);
    T[3].z = -vec4(vertexPosition_modelspace, 1.0).z; //translate to origin before rotation
    mat4 trans =inverse(T)*rotation*T;
    gl_Position = depthMVP*trans*vec4(vertexPosition_modelspace, 1.0);
    UV = UVcoords;
    Z = (depthMVP*rotation*vec4(vertexPosition_modelspace, 1.0)).z;
} 