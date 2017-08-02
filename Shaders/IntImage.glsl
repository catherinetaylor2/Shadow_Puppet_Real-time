#version 330 core

//horizontal pass
in vec2 UV;
layout(location = 0) out vec4 colour;
uniform sampler2D CurrentTexture; 
uniform int textureWidth;
uniform int Ni;
uniform int VerOrHor;

void main(){
    
    if(VerOrHor == 0){ //then horizontal pass
        vec2 UVleft[16];
        for(int i=0; i<16; i++){
            UVleft[i] = UV;
            UVleft[i].x += - (1.0f/float(textureWidth)*float(Ni)*i);
        }
        
        colour = vec4(0,0,0,0);//(texture(CurrentTexture, UV); 
        for(int i=0; i<16; i++){
            colour+=vec4(texture(CurrentTexture, UVleft[i]).rgb,0);
        }
    }
    if(VerOrHor ==1){
        vec2 UVright[16];
        for(int i=0; i<16; i++){
            UVright[i] = UV;
            UVright[i].y += (1.0f/float(textureWidth)*float(Ni)*i);
        }
   
        colour = vec4(0,0,0,0);//(texture(CurrentTexture, UV); 
        for(int i=0; i<16; i++){
            colour+= vec4(texture(CurrentTexture, UVright[i]).rgb, 0);
        }
    }
}