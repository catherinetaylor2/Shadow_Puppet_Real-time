#version 330 core

in vec2 UV; 
in vec3 L;
in vec3 V;

uniform sampler2D renderedTexture; //inputs shadow map and screen texture
uniform sampler2D depthTexture;
uniform sampler2D depthTexture_outer;

out vec3 colour;

void main(){
    vec3 LightDir = 5.0f*V - L;
    vec3 n = vec3(0,0,-1.0f);
    vec3 l = normalize(LightDir);
    float diffuseCoeff = 1.3f*pow(dot(n,l),10.0f);
    colour = texture(renderedTexture, UV).rgb*(1.0f - texture(depthTexture_outer, UV).r/3.5f)*(1.0f - texture(depthTexture,UV).r/1.75f);
}