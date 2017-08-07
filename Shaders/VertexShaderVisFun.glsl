#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform float textureXres;
uniform float textureYres;
uniform mat4 Corners;
uniform mat4 PuppetCorners;

out vec2 dimRatio;
out mat4 LightCornerVertices;
out mat4 QuadCorners;
out vec4 POI;

void main(){
    gl_Position = vec4(vertexPosition_modelspace,1);
    dimRatio = vec2(textureXres, textureYres);
    LightCornerVertices = Corners;
    POI = vec4(vertexPosition_modelspace,1);
    QuadCorners = PuppetCorners;
}