#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include"Read_Obj.hpp"

ObjFile::ObjFile(std::string name){
  ObjFileName = name;
}

void ObjFile::get_vertices(float** V){
  char str[1000];
  float f1, f2, f3;
  std::string s = "a";
  FILE * myObject;
  int vertexCount = 0, t;
  myObject = fopen(ObjFileName.c_str(), "r");

  while (s != "v"){
    t= fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;  
  }
  do{  
    t= fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;
   vertexCount =vertexCount+1;
  }while (s != "vt");

  fclose(myObject);
  myObject = fopen(ObjFileName.c_str(), "r");
  NumberOfVertices =vertexCount;
  *V = new float[3*NumberOfVertices];
  while (s != "v"){
    t= fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;  
  }
  for(int i=0; i<3*NumberOfVertices; i+=3){
    (*V)[i] = f1;
    (*V)[i+1] = f2;
    (*V)[i+2]=f3; 
    t = fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
  }
  fclose(myObject);
}

void ObjFile::get_normals(float** N){
  char str[1000];
  float f1, f2, f3;
  std::string s = "a";
  FILE * myObject;
  int normalCount=0, t;

  myObject = fopen(ObjFileName.c_str(), "r");
  while (s != "vn"){
    t= fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;  
  }
  do{  
    t=fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;
   normalCount= normalCount+1;
  }while (s == "vn");

  fclose(myObject);
  myObject = fopen(ObjFileName.c_str(), "r");
  NumberOfNormals =normalCount;
  *N = new float[3*NumberOfNormals];
  while (s != "vn"){
    t= fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;  
  }
  for(int i=0; i<3*NumberOfNormals; i+=3){
    (*N)[i] = f1;
    (*N)[i+1] = f2;
    (*N)[i+2]=f3;   
    fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
  }
   fclose(myObject);
}

void ObjFile::get_texture(float ** VT){
  char str[1000];
  float f1, f2, f3;
  std::string s = "a";
  FILE * myObject;
  int textureCount = 0;
  int t;
  myObject = fopen(ObjFileName.c_str(), "r");
  t = fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
  s = str; 

  while (s != "v"){
    t= fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;  
  }
  for(int i=0; i< NumberOfVertices-1; i++){
    t=fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;
  }
  do{  
    t=fscanf(myObject, "%s %f %f ", str, &f1, &f2);
    s = str;
   textureCount=textureCount+1;
  }while (s == "vt");

  fclose(myObject);
  myObject = fopen(ObjFileName.c_str(), "r");

  int number_of_text =textureCount-1;
  *VT = new float[2*number_of_text];
  while (s != "v"){
    t= fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;  
  }
  for(int i=0; i< NumberOfVertices-1; i++){
    t=fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;
  }
  for(int i=0; i<2*number_of_text; i+=2){
    t=fscanf(myObject, "%s %f %f ", str, &f1, &f2);
    s=str;
    (*VT)[i] = f1;
    (*VT)[i+1]=f2;
  }
   fclose(myObject);
}

void ObjFile::get_face_data(int** FaceVertices, int** FaceNormals, int** FaceTextures){
  char str[1000], c1, c2, c3, c4, c5, c6;
  float f1, f2, f3;
  int i1, i2, i3, i4, i5, i6, i7, i8, i9;
  std::string s = "a";
  FILE * myObject;
  int faceCount=0, t;

  myObject = fopen(ObjFileName.c_str(), "r");
  t=fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
  s = str; 
  while (s != "vn"){
    t= fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;  
  }
  for(int i=0; i< NumberOfNormals-1; i++){
    t=fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;
  }
  do{    
    s = str;
   faceCount=faceCount+1;
    t= fscanf(myObject, "%s %i %c %i %c %i %i %c %i %c %i %i %c %i %c %i", str, &i1, &c1, &i2, &c2, &i3, &i4, &c3, &i5, &c4, &i6, &i7, &c5, &i8, &c6, &i9);
  }while(t!=EOF);

  fclose(myObject);
  myObject = fopen(ObjFileName.c_str(), "r");
  t= fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
  s = str; 
  while (s != "vn"){
    fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;  
  }
  for(int i=0; i< NumberOfNormals-1; i++){
    t=fscanf(myObject, "%s %f %f %f" , str, &f1, &f2, &f3);
    s = str;
  }
  NumberOfFaces =faceCount-1;
  std::cout<<"Number of mesh faces: "<<NumberOfFaces<<"\n";
  *FaceVertices = new int[3*NumberOfFaces];
  *FaceNormals = new int[3*NumberOfFaces];
  *FaceTextures = new int[3*NumberOfFaces];

  for(int i=0; i<3*NumberOfFaces; i+=3){
    t=fscanf(myObject, "%s %i %c %i %c %i %i %c %i %c %i %i %c %i %c %i", str, &i1, &c1, &i2, &c2, &i3, &i4, &c3, &i5, &c4, &i6, &i7, &c5, &i8, &c6, &i9);
    s=str;
    (*FaceNormals)[i] = i3-1;
    (*FaceNormals)[i+1]=i6-1;
    (*FaceNormals)[i+2]=i9-1;
    (*FaceVertices)[i]=i1-1;
    (*FaceVertices)[i+1]= i4-1;
    (*FaceVertices)[i+2]=i7-1;
    (*FaceTextures)[i]=i2-1;
    (*FaceTextures)[i+1]=i5-1;
    (*FaceTextures)[i+2]=i8-1;
  }
   fclose(myObject);
}
void ObjFile::get_mesh_data(ObjFile mesh,int** FaceVertices, int** FaceNormals, int** FaceTextures, float** texture_coords, float** normals, float** vertices, int* NumberOfFaces, int* NumberOfVertices){
		mesh.get_vertices(vertices);
		mesh.get_texture(texture_coords);
		mesh.get_normals(normals);
		mesh.get_face_data(FaceVertices, FaceNormals, FaceTextures);
	  *NumberOfFaces = mesh.get_NumberOfFaces();
    *NumberOfVertices = mesh.get_NumberOfVertices();
}
void ObjFile::clean_up(float*vertices, float* normals, float* texture_coords,int* FaceVertices, int* FaceNormals, int* FaceTextures){
  delete[] vertices;
  delete[] normals;
  delete[] texture_coords;
  delete[] FaceVertices;
  delete[] FaceNormals;
  delete[] FaceTextures;
}