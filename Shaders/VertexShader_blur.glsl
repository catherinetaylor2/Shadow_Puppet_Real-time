#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;


uniform float widthRatio;
uniform float heightRatio;
uniform mat4 depthMVP;

out vec2 UV;
out vec2 dimRatio;

void main(){
    gl_Position = depthMVP*vec4(vertexPosition_modelspace,1);
    UV = vertexUV;
    dimRatio = vec2(widthRatio, heightRatio);
}