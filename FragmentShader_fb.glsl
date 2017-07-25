#version 330 core 

layout(location = 0) out float gl_FragDepth; 

uniform sampler2D puppet_texture;

in vec2 UV;

void main() { 
  if((texture(puppet_texture, UV).x)==1.0f){
    gl_FragDepth = 1.0f ; 
  }
  else{
    gl_FragDepth =texture(puppet_texture, UV).x*gl_FragCoord.z;
  }
} 