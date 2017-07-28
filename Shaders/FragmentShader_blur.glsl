#version 330 core

layout(location = 0) out vec3 colour; 


in vec2 dimRatio;
in mat4 LightCornerVertices;
in vec4 POI;
in mat4 QuadCorners;

uniform sampler2D renderedTexture; 


vec3 FindIntersectionPoint(vec3 RayDirection, vec3 RayOrigin, vec3 QuadNormal, vec3 QuadPoint){
    float d = dot(RayDirection, QuadNormal);
    if(d < 0.0f){
        return vec3(0,0,0);
    }
    else{
        vec3 p = QuadPoint - RayOrigin;
        float t = dot(p, QuadNormal)/d;
        vec3 IntersectionPoint = RayOrigin + t*RayDirection;   
        return IntersectionPoint;
    }
}

int TestInsideQuad(vec3 POI, vec3 CornerPoint){
    if(POI.z == 0){
        return 0;
    }
    else if((POI.x<CornerPoint.x)||(POI.x>-1*CornerPoint.x)){
        return 0;
    }
    else if((POI.y<-1*CornerPoint.y)||(POI.y>CornerPoint.y)){
        return 0;
    }
    else{
        return 1;
    }
}
vec2 getUV(float xres, float yres, vec3 POI, vec3 lowerCorner, float width, float height){
    float x = lowerCorner.x;
    float y = lowerCorner.y;
    float RatioHor = float(xres)/width;
    vec2 UV = vec2((POI.x-x)/float(width), (POI.y-y)/float(height));
    return UV;
}

int NumberOfPixels(vec2 UV1,vec2 UV2, vec2 UV3, float xres, float yres){
    int a = int(abs(UV2.x - UV1.x)*xres);
    int b = int(abs(UV3.y - UV1.y)*yres);
    int c = a*b;
    return (c);
}
float Visibility(vec2 UV1, vec2 UV2, vec2 UV3, vec2 UV4, int NumberOfPixels, sampler2D tex){
    float S = texture(tex, UV4).r -texture(tex, UV2).r - texture(tex, UV3).r +texture(tex, UV1).r;
    float visfactor = S / float(NumberOfPixels);
    return visfactor;
}

void main(){

vec3 RayDirection [4]; 
for(int i=0;i<4;++i){
    RayDirection[i] = normalize((LightCornerVertices[i] - POI).xyz);
}


vec3 QuadNormal =QuadCorners[3].xyz;
vec3 IntersectionPoints [4];

float width = QuadCorners[1].x - QuadCorners[0].x;
float height = QuadCorners[0].y -  QuadCorners[2].y;
for(int i=0;i<4;++i){
    IntersectionPoints[i] = FindIntersectionPoint(RayDirection[i], POI.xyz, QuadNormal, QuadCorners[0].xyz);
}


int v = 1, c;

for(int i=0;i<4;++i){
    c = TestInsideQuad(IntersectionPoints[i],  QuadCorners[0].xyz);
    v = v*int((c!=0));
}

    if(v==1){

        vec2 UVcoords[4];
        for(int i=0; i<4;++i){
            UVcoords[i] = getUV(dimRatio.x, dimRatio.y, IntersectionPoints[i], QuadCorners[2].xyz, width, height);
        }
        int  i = NumberOfPixels(UVcoords[0],UVcoords[1], UVcoords[2], dimRatio.x, dimRatio.y);
        float vis = Visibility(UVcoords[0],UVcoords[1], UVcoords[2], UVcoords[3], i, renderedTexture);
    
        colour = vec3(vis,0,0); //colour depends on distance between light and screen.

    }
        else{
            colour = vec3(v,0,0);
        }
}