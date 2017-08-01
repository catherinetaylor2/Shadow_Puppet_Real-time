#version 330 core

//vertical pass
in vec2 UV;
layout(location = 0) out vec4 colour;
uniform sampler2D CurrentTexture; 
uniform int textureHeight;
uniform int Mi;

void main(){
    vec2 UVright[16];
    for(int i=0; i<16; i++){
        UVright[i] = UV;
        UVright[i].y += (1.0f/float(textureHeight)*float(Mi)*i);
    }
   
   colour = vec4(0,0,0,0);//(texture(CurrentTexture, UV); 
    for(int i=0; i<16; i++){
        colour+=texture(CurrentTexture, UVright[i]);
    }
    //colour = (texture(CurrentTexture, UV) + texture(CurrentTexture, UVright)).rgb;
}