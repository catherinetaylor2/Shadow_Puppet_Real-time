// Real-time Shadow Puppet Theatre
//
// Created by Catherine Taylor
//
//Began June 2017

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
#include <chrono>
#include <thread>
#include <ctime>


void write_to_shadow_map(GLuint framebuffer,GLuint depthMatrixID, glm::mat4 depthMVP, GLuint puppet_vertexbuffer, GLint posAttrib, int number_of_faces_puppet, GLuint IBO, GLuint rotationID, glm::mat4 rotation, GLuint puppet_textureID, GLuint UVbuffer){
  
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); //bind to shadow map
    glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]); //load in MVP matrix
    glUniformMatrix4fv(rotationID, 1, GL_FALSE, &rotation[0][0]); //rotate image if wanted
    glActiveTexture(GL_TEXTURE0); //load in puppet texture
    glBindTexture(GL_TEXTURE_2D, puppet_textureID);
    glEnable(GL_DEPTH_TEST); //find depth values
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, puppet_vertexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(  0, // 0  is vertex
                            3, //size of information
                            GL_FLOAT, // type of the data
                            GL_FALSE, // normalised?
                            0, // stride
                            0 // offset
                        );   
    glEnableVertexAttribArray(1); //use UV coords
    glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
    glVertexAttribPointer(  1,
                            2,
                            GL_FLOAT,
                            GL_FALSE,
                            0,  
                            0
                        );

    glDrawElements(GL_TRIANGLES, 3*number_of_faces_puppet,  GL_UNSIGNED_INT,0); // draw mesh
    glDisableVertexAttribArray(0);
};

int main(int argc, char* argv[] ){

    int width, height;
	if(argc>1){
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}
	else{
		width = 500;
		height = 500;
	}

//screen texture data
    unsigned char * texture_data;
	int texture_width, texture_height;
	texture_data = readBMP("screen_texture.bmp", &texture_width, &texture_height);

//puppet texture data
    unsigned char *puppet_data;
    int puppet_width, puppet_height;
    puppet_data = readBMP("dino_texture.bmp", &puppet_width, &puppet_height);

//Input mesh of puppet as obj file
    float *Vx_puppet, *N_puppet, *VT_puppet;
    int number_of_faces_puppet, *FV_puppet, *FN_puppet, *FT_puppet, number_of_vertices_puppet;
    ObjFile mesh_puppet("quad.obj");
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

//create openGL window
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

    GLuint VertexArrayID; //Create vertex array object and set to be current
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
//CAMERA POSITION DATA-----------------------------------------------------------------------------------------
    // glm::mat4 ViewMatrix =  glm::lookAt( glm::vec3(0,0,-4), // position of camera
    //                             glm::vec3(0,0,1),  // look at vector
    //                             glm::vec3(0,1,0)  //look up vector
    //                         );
    // glm::mat4 ModelMatrix = glm::mat4(1.0f); //Create MVP matrices.
    // ModelMatrix[3].w = 1.0;
    // glm::mat4 projectionMatrix = glm::perspective(
    //     glm::radians (45.0f),         //FOV
    //     (float)width/(float)height, // Aspect Ratio. 
    //     0.1f,        // Near clipping plane. 
    //     100.0f       // Far clipping plane.
    // );
    // glm::mat4 MVP = projectionMatrix*ViewMatrix*ModelMatrix; //NOT USED CURRENTLY

//-------------------------------------------------------------------------------------------------------------
//INPUT TEXTURES:

    GLuint puppet_textureID; //create texture from screen image
    glGenTextures(1, &puppet_textureID);
    glBindTexture(GL_TEXTURE_2D, puppet_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, puppet_width, puppet_height, 0, GL_RGB, GL_UNSIGNED_BYTE, puppet_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint textureID; //create texture from puppet image
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); //set keys for window

//------------------
//COLOR BUFFER:
    GLuint fb;
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    GLuint screen_tex; // generate textures which will contain depth info
    glGenTextures(1, &screen_tex);
    glBindTexture(GL_TEXTURE_2D, screen_tex); //set up inner shadow map
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, screen_tex, 0); 
    

      if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //check depth buffer is complete
        std::cout << "Error: frame buffer not complete \n" ;


 // Depth Buffer code: --------------------------------------------------------------------------------------------------------------------------

    GLuint framebuffer[2]; //create 2 buffers to produce shadow maps
    glGenFramebuffers(2, framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[1]);  //inner light buffer

    GLuint depthTexture[2]; // generate textures which will contain depth info
    glGenTextures(2, depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture[1]); //set up inner shadow map
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture[1], 0);  
    glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //check depth buffer is complete
        std::cout << "Error: frame buffer not complete \n" ;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer[0]); //set up outer light buffer

    glBindTexture(GL_TEXTURE_2D, depthTexture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture[0], 0);  
    glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) //check depth buffer is complete
        std::cout << "Error: frame buffer not complete \n" ;

//light data
    glm::vec3 lightInvDir = glm::vec3(0.0f, 0, -10); //find objects which occlude the light source
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-5.5,5.5,-5.5,5.5,-10,12); //size of dino visiible
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,-1), glm::vec3(0,1,0));
    glm::mat4 depthModelMatrix =  glm::mat4(1.0f);
    depthModelMatrix[3].w=1.0f;
    glm::mat4 depthMVP = depthProjectionMatrix*depthViewMatrix*depthModelMatrix;

    glm::mat4 depthProjectionMatrix_outer = glm::ortho<float>(-5,5,-5,5,-10,15); //size of dino visiible
    glm::mat4 depthMVP_outer = depthProjectionMatrix_outer*depthViewMatrix*depthModelMatrix;

//-----------------------------------------------------------------------------------------------------------------------------------
// PUPPET DATA:

    GLuint puppet_vertexbuffer; //buffer containing puppet mesh
    glGenBuffers(1, &puppet_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, puppet_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, 3*number_of_vertices_puppet*sizeof(float), &Vx_puppet[0], GL_DYNAMIC_DRAW);

    unsigned int* indices = new unsigned int [3*number_of_faces_puppet]; // create array containing position of vertices.
    for(int i=0; i<3*number_of_faces_puppet; i+=3){
        indices[i]=FV_puppet[i]-1;
        indices[i+1]=FV_puppet[i+1]-1;
        indices[i+2]=FV_puppet[i+2]-1;   
    }

    GLuint IBO; //index buffer
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*number_of_faces_puppet*sizeof(unsigned int), indices, GL_DYNAMIC_DRAW); 

    GLuint UVbuffer;
    glGenBuffers(1, &UVbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, UVbuffer);
    glBufferData(GL_ARRAY_BUFFER,2*number_of_vertices_puppet*sizeof(float),  &VT_puppet[0], GL_STATIC_DRAW);

  
   
//-----------------------------------------------------------------------------------------------------------------------------------------------------
// Screen texture data:

    float ver [] = { //quad filling whole screen
        -1.0f,-1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f,1.0f, 0.0f,
    };

    float col [] = { //UV coords
        0.0f, 0.0f, 
        1.0f, 0.0f, 
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f, 
        0.0f, 1.0f,
    };

    GLuint vertexbuffer; //create buffers for screen values
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ver), ver, GL_DYNAMIC_DRAW);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(col),  &col, GL_STATIC_DRAW);

    float rot_angle = 0.0f;

    glm::mat4 rotation = {cos(rot_angle), sin(rot_angle), 0.0f, 0.0f,
                        - sin(rot_angle), cos(rot_angle),0.0f, 0.0f,
                        0.0f, 0.0f,1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f,1.0f,};
//---------------------------------------------------------------------------------------------------------------------------------------------------
//LOAD SHADERS:
    GLuint programID = LoadShaders("VertexShader.vertexshader", "FragmentShader.fragmentshader"); //load screen shaders
    GLuint texID= glGetUniformLocation(programID, "renderedTexture"); //three textures inputted to fragment shader
    GLuint depthID_inner = glGetUniformLocation(programID, "depthTexture");
    GLuint depthID_outer =  glGetUniformLocation(programID, "depthTexture_outer");
    glUseProgram(programID); 
    glUniform1i(texID, 0);
    glUniform1i(depthID_inner, 1);
    glUniform1i(depthID_outer, 2);

    GLuint depthprogramID = LoadShaders("VertexShader_fb.vertexshader", "FragmentShader_fb.fragmentshader"); //load shaders
    GLuint depthMatrixID = glGetUniformLocation(depthprogramID, "depthMVP"); //load MVP matrix to shader
    GLuint rotationID = glGetUniformLocation( depthprogramID, "rotation");
    GLuint puppetID = glGetUniformLocation(depthprogramID, "puppet_texture");
    glUseProgram(depthprogramID); 
    glUniform1i(puppetID, 0);

    GLuint screenprogramID = LoadShaders("VertexShader_cb.vertexshader", "FragmentShader_cb.fragmentshader"); //load shaders
    GLuint screenID = glGetUniformLocation(screenprogramID, "screenTexture");
    glUseProgram(screenprogramID); 
    glUniform1i(screenID, 0);



    glUseProgram(screenprogramID); //create screen texture with lighting model
    glActiveTexture(GL_TEXTURE0); //load in screen texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

        glEnableVertexAttribArray(0); //draw quad with textures mapped on.
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    GLint posAttribs = glGetAttribLocation(screenprogramID, "position");
    glEnableVertexAttribArray(posAttribs);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,  
        (void*)0
    );
    glEnableVertexAttribArray(1); //use UV coords
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

    std::chrono::time_point<std::chrono::system_clock>start,end;
    do{ //while window is open


        start = std::chrono::system_clock::now();

        //Render to shadow maps:
        glUseProgram(depthprogramID); //use shadow map shaders       
        GLint posAttrib_shadow = glGetAttribLocation(depthprogramID, "position"); 
        write_to_shadow_map(framebuffer[1],depthMatrixID, depthMVP, puppet_vertexbuffer, posAttrib_shadow, number_of_faces_puppet,IBO,rotationID, rotation, puppet_textureID, UVbuffer);
        write_to_shadow_map(framebuffer[0],depthMatrixID, depthMVP_outer, puppet_vertexbuffer, posAttrib_shadow, number_of_faces_puppet,IBO,rotationID,rotation, puppet_textureID, UVbuffer);

        

        glBindFramebuffer(GL_FRAMEBUFFER, 0); //bind to default frame buffer
        glViewport(0,0,width,height); //fill whole screen

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID); //use screen shaders
        glActiveTexture(GL_TEXTURE0); //load in screen texture
        glBindTexture(GL_TEXTURE_2D, screen_tex);
        glActiveTexture(GL_TEXTURE1); //load in shadow map
        glBindTexture(GL_TEXTURE_2D, depthTexture[1]);
        glActiveTexture(GL_TEXTURE2); //load in shadow map
        glBindTexture(GL_TEXTURE_2D, depthTexture[0]);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnableVertexAttribArray(0); //draw quad with textures mapped on.
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
        glEnableVertexAttribArray(1); //use UV coords
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

       rot_angle += 0.01f; 
        rotation = {cos(rot_angle), sin(rot_angle), 0.0f, 0.0f,
                    - sin(rot_angle), cos(rot_angle),0.0f, 0.0f,
                    0.0f, 0.0f,1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,1.0f,};
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_time = end - start;
//    std::cout<<"frame rate "<<1.0f/elapsed_time.count()<<"\n";

    }while(glfwGetKey(window, GLFW_KEY_ESCAPE)!=GLFW_PRESS && glfwWindowShouldClose(window)==0); //close if escape pressed

//clear up and delete buffers
    delete[] texture_data;
    delete[] puppet_data;
    delete[] indices;
    ObjFile::clean_up(Vx_puppet,N_puppet, VT_puppet, FV_puppet, FN_puppet, FT_puppet);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &puppet_vertexbuffer);
    glDeleteBuffers(1, &UVbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteBuffers(1, &depthTexture[1]);
    glDeleteBuffers(1, &depthTexture[0]);
    glDeleteBuffers(1, &puppet_textureID);
    glDeleteBuffers(1, &textureID);
    glDeleteBuffers(1, &IBO);
    glDeleteFramebuffers(1, &framebuffer[1]);  
    glDeleteFramebuffers(1, &framebuffer[0]);  

    return 0;
}