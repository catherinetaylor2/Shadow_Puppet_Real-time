#version 330 core

layout(location = 0) out vec3 colour; 

in vec2 dimRatio;
in mat4 LightCornerVertices;
in vec4 POI;
in mat4 QuadCorners;
in vec2 st;

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

vec2 getUV(vec3 POI, vec3 lowerCorner, float width, float height){
    float x = lowerCorner.x;
    float y = lowerCorner.y;
    vec2 UV = vec2((POI.x-x)/float(width), (POI.y-y)/float(height));
    return UV;
}

int NumberOfPixels(vec2 UV[4], vec2 res){
    int a = int(abs(UV[1].x - UV[0].x)*res.x);
    int b = int(abs(UV[2].y - UV[0].y)*res.y);
    int c = a*b;
    return (c);
}
float BilinearInterpolation(vec2 UV, sampler2D tex, vec2 res){
   int Vertex1x,Vertex1y, Vertex2x, Vertex4y;
   float u = UV.x*res.x;
   float v = UV.y*res.y;
    Vertex1x = int(floor(u)); //find corner pixel values
    Vertex1y = int(ceil(v));
    Vertex2x = int(ceil(u));
    Vertex4y = int(floor(v));

    float alpha = float((u - (Vertex2x - Vertex1x)*Vertex1x))/float((Vertex2x - Vertex1x));
    float beta = float((v - (Vertex1y - Vertex4y)*Vertex4y))/float((Vertex1y - Vertex4y));

    float val12 = (1-alpha)*texture(tex, vec2(Vertex1x/res.x, Vertex1y/res.y)).r + alpha*texture(tex, vec2(Vertex2x/res.x, Vertex1y/res.y)).r;
    float val34 = (1-alpha)*texture(tex, vec2(Vertex1x/res.x, Vertex4y/res.y)).r + alpha*texture(tex, vec2(Vertex2x/res.x, Vertex4y/res.y)).r;
    float val = (1-beta)*val12 + beta*val34;

    return val;
}
float Visibility(vec2 UV[4], int NumberOfPixels, sampler2D tex, vec2 res){

    for(int i=0; i<4; i++){
        UV[i].x = UV[i].x - 1.0f/res.x*float((i+1)%2);
        UV[i].x = (UV[i].x)*float(UV[i].x>0.0f)*float(UV[i].x<1.0f)+float(UV[i].x>1.0f);
        UV[i].y = UV[i].y - 1.0f/res.y*float(i<2);
        UV[i].y = (UV[i].y)*float(UV[i].y>0.0f)*float(UV[i].y<1.0f)+float(UV[i].y>1.0f);

    }

 // float S = BilinearInterpolation(UV[3], tex, res) -BilinearInterpolation(UV[1], tex,res) - BilinearInterpolation(UV[2], tex, res) +BilinearInterpolation(UV[0], tex, res);
   float S = texture(tex, UV[3]).r - texture(tex, UV[1]).r -texture(tex, UV[2]).r +texture(tex, UV[0]).r;
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
    
   

    vec2 UVcoords[4];
    for(int i=0; i<4;++i){
        UVcoords[i] = getUV(IntersectionPoints[i], QuadCorners[2].xyz, width, height);
    }
    float dist = abs(texture(renderedTexture, st).w );
    int  i = NumberOfPixels(UVcoords, dimRatio);
    float vis = Visibility( UVcoords,i, renderedTexture, dimRatio);
  //  if(dist ==1){
     //   colour = vec3(0,255,0);
  //  }
    //else{
    colour = vec3(vis,0,0); //colour depends on distance between light and screen.
    //}
    
}