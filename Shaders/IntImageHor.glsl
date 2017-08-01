#version 330 core

//horizontal pass
in vec2 UV;
layout(location = 0) out vec4 colour;
uniform sampler2D CurrentTexture; 
uniform int textureWidth;
uniform int Ni;

void main(){
    vec2 UVleft[16];
    for(int i=0; i<16; i++){
        UVleft[i] = UV;
        UVleft[i].x += - (1.0f/float(textureWidth)*float(Ni)*i);
    }
    
    colour = vec4(0,0,0,0);//(texture(CurrentTexture, UV); 
    for(int i=0; i<16; i++){
        colour+=texture(CurrentTexture, UVleft[i]);
    }
}