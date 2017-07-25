#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 uvCoords;

uniform vec3 LightPos;

out vec2 UV;
out vec3 L;
out vec3 V;

void main(){
    gl_Position = vec4(vertexPosition_modelspace, 1.0);
    UV = uvCoords;
    L = LightPos;
    V = vertexPosition_modelspace;
} 