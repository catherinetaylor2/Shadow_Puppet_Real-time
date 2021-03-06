#version 330 core 

layout(location = 0) out vec3 color; 

uniform sampler2D screenTexture;
in vec2 UV;
in vec3 L;
in vec3 V;

void main() { 
  vec3 LightDir = 5*V - L;
  vec3 l = normalize(LightDir);
  float diffuseCoeff = pow(dot(vec3(0,0,-1.0f),l),50.0f);
  color = diffuseCoeff*vec3(1.1*texture(screenTexture, UV).r, texture(screenTexture, UV).g, texture(screenTexture, UV).b);
} 