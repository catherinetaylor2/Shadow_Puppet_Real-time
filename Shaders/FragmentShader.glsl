#version 330 core

in vec2 UV; 
in vec3 L;
in vec3 V;

uniform sampler2D renderedTexture; //inputs shadow map and screen texture
uniform sampler2D depthTexture;
uniform sampler2D depthTexture_outer;
uniform sampler2D depth;

out vec3 colour;

void main(){
    vec3 LightDir = 5.0f*V - L;
    vec3 n = vec3(0,0,-1.0f);
    vec3 l = normalize(LightDir);
    float diffuseCoeff = 1.3f*pow(dot(n,l),10.0f);
    float depthVal = 1 - texture(depth, UV).w;
    float InnerVal = min((depthVal*texture(depthTexture_outer, UV).r/4.0f), 1.0f);
    float OuterVal = min(texture(depthTexture,UV).r*depthVal, 1.0f);
    
    colour = diffuseCoeff*texture(renderedTexture, UV).rgb*(1.0f - InnerVal)*(1.0f - OuterVal);

}