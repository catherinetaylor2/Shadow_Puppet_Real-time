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
#include "BITMAP.hpp"
#include"Read_Obj.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

int main(int argc, char* argv[] ){

    int width, height;
	if(argc>1){
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}
	else{
		width=500;
		height=500;
	}

    unsigned char * texture_data;
	int texture_width, texture_height;
	texture_data = readBMP("screen_texture.bmp", &texture_width, &texture_height);

    unsigned char *puppet_data;
    int puppet_width, puppet_height;
    puppet_data = readBMP("dino_texture.bmp", &puppet_width, &puppet_height);

    float *Vx_puppet, *N_puppet, *VT_puppet;
    int number_of_faces_puppet, *FV_puppet, *FN_puppet, *FT_puppet, number_of_vertices_puppet;
    ObjFile mesh_puppet("dino_puppet_handle.obj");
	mesh_puppet.get_mesh_data(mesh_puppet, &FV_puppet, &FN_puppet, &FT_puppet, &VT_puppet, &N_puppet, &Vx_puppet, &number_of_faces_puppet, &number_of_vertices_puppet);
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
    window = glfwCreateWindow(width, height, "Shadow Puppet", NULL, NULL);
    
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
    
//CAMERA POSITION DATA-----------------------------------------------------------------------------------------
    glm::vec3 LightPos = glm::vec3(0.0f,0.0f, 50.0f);
    glm::mat4 V =  glm::lookAt(
                            glm::vec3(0,0,-4), // position of camera
                            glm::vec3(0,0,1),  // look at vector
                            glm::vec3(0,1,0)  //look up vector
    );
    glm::mat4 M = glm::mat4(0.65f); //Create MVP matrices.
    M[3].w = 1.0;
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians (45.0f),         //FOV
        (float)width/(float)height, // Aspect Ratio. 
        0.1f,        // Near clipping plane. 
        100.0f       // Far clipping plane.
    );
    glm::mat4 MVP = projectionMatrix*V*M;
//-------------------------------------------------------------------------------------------------------------
    GLuint puppet_textureID;
    glGenTextures(1, &puppet_textureID);
    glBindTexture(GL_TEXTURE_2D, puppet_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, puppet_width, puppet_height, 0, GL_RGB, GL_UNSIGNED_BYTE, puppet_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
 // FRAME BUFFER CODE: --------------------------------------------------------------------------------------------------------------------------

    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);    
    // generate texture
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);  

    glDrawBuffer(GL_NONE);

    // unsigned int rbo;
    // glGenRenderbuffers(1, &rbo);
    // glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  
    // glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! \n" ;
    // glBindFramebuffer(GL_FRAMEBUFFER, 0); 


glm::vec3 lightInvDir = glm::vec3(0.0f, 2, -10);

glm::mat4 depthProjectionMatrix = glm::ortho<float>(-7,7,-7,7,-10,20);
glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
glm::mat4 depthModelMatrix =  glm::mat4(1.0f);
glm::mat4 depthMVP = depthProjectionMatrix*depthViewMatrix*depthModelMatrix;


    GLuint puppet_vertexbuffer;
    glGenBuffers(1, &puppet_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, puppet_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, 3*number_of_vertices_puppet*sizeof(float), Vx_puppet, GL_DYNAMIC_DRAW);

    unsigned int* indices = new unsigned int [3*number_of_faces_puppet]; // create array containing position of vertices.
    for(int i=0; i<3*number_of_faces_puppet; i+=3){
        indices[i]=FV_puppet[i]-1;
        indices[i+1]=FV_puppet[i+1]-1;
        indices[i+2]=FV_puppet[i+2]-1;   
    }

    GLuint IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*number_of_faces_puppet*sizeof(unsigned int), indices, GL_DYNAMIC_DRAW); 

    GLuint depthprogramID = LoadShaders("VertexShader_fb.vertexshader", "FragmentShader_fb.fragmentshader");
    GLuint depthMatrixID = glGetUniformLocation(depthprogramID, "depthMVP");


 //-----------------------------------------------------------------------------------------------------------------------------------------------   

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// Screen texture data:

    
    float ver [] = {
        -1.0f,-1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f,1.0f, 0.0f,
    };

    float col [] = {
        0.0f, 0.0f, 
        1.0f, 0.0f, 
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f, 
        0.0f, 1.0f,
    };


    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ver), ver, GL_DYNAMIC_DRAW);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(col),  &col, GL_STATIC_DRAW);

   glm::mat4 biasMatrix(0.5,0.0,0.0, 0.0,
   0.0, 0.5, 0.0, 0.0,
   0.0, 0.0, 0.5, 0.0,
   0.5, 0.5, 0.5, 1.0
   );
   glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
//---------------------------------------------------------------------------------------------------------------------------------------------------

    GLuint programID = LoadShaders("VertexShader.vertexshader", "FragmentShader.fragmentshader");
    GLuint MatrixID = glGetUniformLocation(programID, "depthBiasMVP");

    GLuint texID= glGetUniformLocation(programID, "textureID"); //two textures inputted to fragment shader
    GLuint depthID = glGetUniformLocation(programID, "depthTexture");
    glUseProgram(programID);
    glUniform1i(texID, 0);
    glUniform1i(depthID, 1);


    do{
        //FRAME BUFFER:
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glUseProgram(depthprogramID);
        glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, puppet_vertexbuffer);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        GLint posAttrib1 = glGetAttribLocation(depthprogramID, "position");
        glEnableVertexAttribArray(posAttrib1);
        glVertexAttribPointer(0, // 0  is vertex
                             3, //size of information
                             GL_FLOAT, // type of the data
                             GL_FALSE, // normalised?
                             0, // stride
                             0 // offset
        );        
        glDrawElements(GL_TRIANGLES, 3*number_of_faces_puppet,  GL_UNSIGNED_INT,0); // draw mesh



        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,width,height);
        //------------------------------------------------------------------------------------

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);
        glActiveTexture(GL_TEXTURE0);
       glBindTexture(GL_TEXTURE_2D, textureID);
        glActiveTexture(GL_TEXTURE1);
       glBindTexture(GL_TEXTURE_2D, depthTexture);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &depthBiasMVP[0][0]);

       glEnable(GL_DEPTH_TEST);
       glDepthFunc(GL_LESS);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        GLint posAttrib = glGetAttribLocation(programID, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,  
            (void*)0
        );
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,  
            (void*)0
        );
        glDrawArrays(GL_TRIANGLES,0,6);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    while(glfwGetKey(window, GLFW_KEY_ESCAPE)!=GLFW_PRESS && glfwWindowShouldClose(window)==0);

    delete[] texture_data;
    delete[] puppet_data;
    delete[] indices;
    ObjFile::clean_up(Vx_puppet,N_puppet, VT_puppet, FV_puppet, FN_puppet, FT_puppet);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
  //  glDeleteBuffers(1,&rbo);
    glDeleteBuffers(1,&depthTexture);
    glDeleteBuffers(1, &puppet_textureID);
    glDeleteBuffers(1, &textureID);
    glDeleteFramebuffers(1, &framebuffer);  

    return 0;
}