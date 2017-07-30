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
float x1 = (UV1.x-1)*float(UV1.x>0.0f)*float(UV1.x<1.0f)+float(UV1.x>1.0f); //optimize this
float x2 = UV2.x*float(UV1.x>0.0f)*float(UV3.x<1.0f)+float(UV2.x>1.0f);
float x3 = (UV3.x-1)*float(UV1.x>0.0f)*float(UV3.x<1.0f)+float(UV3.x>1.0f);
float x4 = UV4.x*float(UV1.x>0.0f)*float(UV4.x<1.0f)+float(UV4.x>1.0f);
float y1 = (UV1.y-1)*float(UV1.y>0.0f)*float(UV1.y<1.0f)+float(UV1.y>1.0f);
float y2 = (UV2.y-1)*float(UV2.y>0.0f)*float(UV2.y<1.0f)+float(UV2.y>1.0f);
float y3 = UV3.y*float(UV3.y>0.0f)*float(UV3.y<1.0f)+float(UV3.y>1.0f);
float y4 = UV4.y*float(UV4.y>0.0f)*float(UV4.y<1.0f)+float(UV4.y>1.0f);

    float S = texture(tex, vec2(x4,y4)).r -texture(tex, vec2(x2,y2)).r - texture(tex, vec2(x3,y3)).r +texture(tex, vec2(x1,y1)).r;
    float visfactor = S / float(NumberOfPixels);
    return visfactor;
}

void main(){

    vec3 QuadNormal =QuadCorners[3].xyz;
    float width = QuadCorners[1].x - QuadCorners[0].x;
    float height = QuadCorners[0].y -  QuadCorners[2].y;
    vec3 IntersectionPoints [4];
    vec3 RayDirection [4]; 

    for(int i=0;i<4;++i){
        RayDirection[i] = normalize((LightCornerVertices[i] - POI).xyz);
        IntersectionPoints[i] = FindIntersectionPoint(RayDirection[i], POI.xyz, QuadNormal, QuadCorners[0].xyz);
    }
    float IntersectionZ = 0.25f*(IntersectionPoints[0].z+IntersectionPoints[1].z+IntersectionPoints[2].z+IntersectionPoints[3].z);
    float dist = 1 - (IntersectionZ - POI.z)/100.0f;
   

        vec2 UVcoords[4];
        for(int i=0; i<4;++i){
            UVcoords[i] = getUV(dimRatio.x, dimRatio.y, IntersectionPoints[i], QuadCorners[2].xyz, width, height);
        }
        int  i = NumberOfPixels(UVcoords[0],UVcoords[1], UVcoords[2], dimRatio.x, dimRatio.y);
        float vis = Visibility(UVcoords[0],UVcoords[1], UVcoords[2], UVcoords[3], i, renderedTexture);
        colour = vec3(dist*vis,0,0); //colour depends on distance between light and screen.
    
}