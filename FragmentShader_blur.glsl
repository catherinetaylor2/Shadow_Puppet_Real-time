#version 330 core

layout(location = 0) out vec3 colour; 

in vec2 UV; 
in vec2 dimRatio;

uniform sampler2D renderedTexture; 

#define min 7 //changes level of blur

void main(){
    float a,b, occ = 0.0f;
    vec2  u_v;
    for(int i=-min; i<min+1;++i){
        for(int j=-min; j<min+1; ++j){
            a=i*dimRatio.x;
            b=j*dimRatio.y;
            u_v = UV + vec2(a,b);
            bool temp = texture(renderedTexture, u_v).x <1.0f;
            occ += float(temp);
        }
    }
    float val = (1.0f - occ/(4*min*min*2));
    colour = vec3(val,val,val); //colour depends on distance between light and screen.
}