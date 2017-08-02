#version 330 core 

layout(location = 0) out vec4 colour; 

uniform sampler2D puppet_texture;

in vec2 UV;
in float Z;

void main() { 
  float depth =  (Z-5)/13.0f; //SORT THIS
  colour = vec4(1,1,1,depth) -vec4(texture(puppet_texture, UV).rgb,0);
} 