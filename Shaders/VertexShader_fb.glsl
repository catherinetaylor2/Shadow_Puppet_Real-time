#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 UVcoords;

uniform mat4 depthMVP; 
uniform mat4 rotation;

out vec2 UV;
out float Z;

void main(){
    mat4 TempdepthMVP = 1.0f/vertexPosition_modelspace.z*depthMVP;
    TempdepthMVP[3].w = 1.0f;
    gl_Position = rotation*depthMVP*vec4(vertexPosition_modelspace, 1.0);
    UV = UVcoords;
    Z = vertexPosition_modelspace.z;
} 