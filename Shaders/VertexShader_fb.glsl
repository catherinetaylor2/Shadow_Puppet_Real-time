#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 UVcoords;

uniform mat4 depthMVP; 
uniform mat4 rotation;


out vec2 UV;

void main(){
    gl_Position = rotation*depthMVP*vec4(vertexPosition_modelspace, 1.0); //make this screen pos?
    UV = UVcoords;
} 