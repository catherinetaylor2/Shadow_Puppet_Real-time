#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 UVcoords;

uniform mat4 depthMVP; 
uniform mat4 rotation;

out vec2 UV;
out float Z;

void main(){
    gl_Position = depthMVP*rotation*vec4(vertexPosition_modelspace, 1.0);
    UV = UVcoords;
    Z = vertexPosition_modelspace.z;
} 