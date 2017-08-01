#version 330 core 

layout(location = 0) out vec4 colour; 

uniform sampler2D puppet_texture;

in vec2 UV;
in float Z;

void main() { 
  float depth =1.0f;
  if((texture(puppet_texture, UV).x)==0.0f){
 depth=1.0f/Z; 
 }
  // else{
  //   gl_FragDepth =texture(puppet_texture, UV).x*gl_FragCoord.z;
  // }
  colour = vec4(1,1,1,depth) -vec4(texture(puppet_texture, UV).rgb,0);
 // colour = vec4(Z,0,0,0);
} 