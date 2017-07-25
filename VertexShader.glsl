#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform vec3 LightPos;
uniform float widthRatio;
uniform float heightRatio;

out vec2 UV;
out vec3 L;
out vec3 V;
out vec2 dimRatio;

void main(){
    gl_Position =vec4(vertexPosition_modelspace,1);
    UV = vertexUV;
    L = LightPos;
    V = vertexPosition_modelspace;
    dimRatio = vec2(widthRatio, heightRatio);
}