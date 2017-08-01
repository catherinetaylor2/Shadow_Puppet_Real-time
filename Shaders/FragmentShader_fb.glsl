#version 330 core 

layout(location = 0) out vec3 colour; 

uniform sampler2D puppet_texture;

in vec2 UV;

void main() { 
  // if((texture(puppet_texture, UV).x)==1.0f){
  //   gl_FragDepth = 1.0f ; 
  // }
  // else{
  //   gl_FragDepth =texture(puppet_texture, UV).x*gl_FragCoord.z;
  // }
  colour = vec3(1,1,1) - texture(puppet_texture, UV).rgb;
} 