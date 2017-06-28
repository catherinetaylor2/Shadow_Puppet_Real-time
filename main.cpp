#include <glew.h>
#include <glfw3.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <cstdio>
#include <vector>
#include "shader.hpp"
#include "Read_Obj.hpp"
#include "BITMAP.hpp"
#include <glm/gtx/transform.hpp>

int main(){

    unsigned char * texture_data;
	int texture_width, texture_height;
	texture_data = readBMP("sheet3.bmp", &texture_width, &texture_height);GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    float *V, *N, *VT;
    int number_of_faces, *FV, *FN, *FT, number_of_vertices;
    ObjFile mesh("plane.obj");
	mesh.get_mesh_data(mesh, &FV, &FN, &FT, &VT, &N, &V, &number_of_faces, &number_of_vertices);
	std::cout<<"tree built \n";

    if(!glfwInit()){ // initialize GLFW
        std::cout<<"Error: failed to initialize GLFW \n";
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;
    int width = 500, height = 500;
    window = glfwCreateWindow(width, height, "Real-time Shadows", NULL, NULL);
    if(window==NULL){
        std::cout<<"Error: failed to open window";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if(glewInit()!=GLEW_OK){
        std::cout<<"Error: failed to initialize GLEW \n";
        return -1;
    }
     do{        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }while(glfwGetKey(window, GLFW_KEY_ESCAPE)!=GLFW_PRESS && glfwWindowShouldClose(window)==0);


    delete [] texture_data;
    ObjFile::clean_up(V,N, VT, FV, FN, FT);
    return 0;
}