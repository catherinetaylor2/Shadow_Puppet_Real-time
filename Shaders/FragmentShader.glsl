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
    float InnerdepthVal = (1.0f - texture(depth, vec2(1-UV.x, UV.y)).w)/1.5f;
    float depthVal = texture(depth, vec2(1-UV.x, UV.y)).w;
    float OuterVal = min(((1-depthVal)/1.5f*texture(depthTexture_outer, vec2(1-UV.x, UV.y)).r), 1.0f);
    float InnerVal =  min(InnerdepthVal*3.0f*texture(depthTexture,vec2(1-UV.x, UV.y)).r/1.05f, 1.0f);
    
    colour = diffuseCoeff*texture(renderedTexture, vec2(1-UV.x, UV.y)).rgb*(1.0f - InnerVal)*(1.0f - OuterVal);

}