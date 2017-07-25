// Real-time Shadow Puppet Theatre
//
// Created by Catherine Taylor
//
//Began June 2017

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <cmath>
#include "shader.hpp"
#include "BITMAP.hpp"
#include"Read_Obj.hpp"
#include <vector>
#include "initializing.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

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
    float widthRatio = 1.0f/(float)width, heightRatio = 1.0f/(float)height; //used in blurring calculations

    unsigned char * texture_data; //screen texture data
	int texture_width, texture_height;
	texture_data = readBMP("sheet.bmp", &texture_width, &texture_height);

    unsigned char *puppet_data; //puppet texture data
    int puppet_width, puppet_height;
    puppet_data = readBMP("dino_texture.bmp", &puppet_width, &puppet_height);

    float *Vx_puppet, *N_puppet, *VT_puppet; //Input mesh of puppet as obj file
    int number_of_faces_puppet, *FV_puppet, *FN_puppet, *FT_puppet, number_of_vertices_puppet;
    ObjFile mesh_puppet("quad.obj");
	mesh_puppet.get_mesh_data(mesh_puppet, &FV_puppet, &FN_puppet, &FT_puppet, &VT_puppet, &N_puppet, &Vx_puppet, &number_of_faces_puppet, &number_of_vertices_puppet);

    float *Vx, *N, *VT; //Input plane with lots of triangles
    int number_of_faces, *FV, *FN, *FT, number_of_vertices;
    ObjFile mesh_screen("plane.obj");
	mesh_screen.get_mesh_data(mesh_screen, &FV, &FN, &FT, &VT, &N, &Vx, &number_of_faces, &number_of_vertices);

	std::cout<<"Inputed files loaded \n";

    if(!glfwInit()){ // initialize GLFW
        std::cerr<<"Error: failed to initialize GLFW \n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES,4); // select openGL version settings 
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
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); //set keys for window

//INITIAL POSITION DATA-----------------------------------------------------------------------------------------
   
//light data
    glm::vec3 LightPos = glm::vec3(0.0f,0.0f,40.0f);
    glm::vec3 lightInvDir = glm::vec3(0.0f, 0, -10); //find objects which occlude the light source
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-5.5,5.5,-5.5,5.5,-10,12); //size of dino visible from inner light
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,-1), glm::vec3(0,1,0));
    glm::mat4 depthMVP = depthProjectionMatrix*depthViewMatrix;
    glm::mat4 depthProjectionMatrix_outer = glm::ortho<float>(-5,5,-5,5,-10,15); //size of dino visible from outer light
    glm::mat4 depthMVP_outer = depthProjectionMatrix_outer*depthViewMatrix;

    float rot_angle = 0.0f; //set up initial rotation matrix
    glm::mat4 rotation = {  cos(rot_angle), sin(rot_angle), 0.0f, 0.0f,
                            - sin(rot_angle), cos(rot_angle),0.0f, 0.0f,
                            0.0f, 0.0f,1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f,1.0f,};

// Screen texture data:
    float ver [] = { //quad filling whole screen with only 2 triangles
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

//-----------------------------------------------------------------------------------------------------------
//CREATE FRAMEBUFFERS AND TEXTURES: 

    GLuint textureID[2]; //create texture from inputted bitmaps
    glGenTextures(2, textureID);
    initialize_texture(textureID[0], texture_data, texture_width, texture_height);
    initialize_texture(textureID[1], puppet_data, puppet_width, puppet_height);

    GLuint framebuffer[4]; //create 4 framebuffers
    glGenFramebuffers(4, framebuffer);
    GLuint depthTexture[4]; // generate 4 textures which will be written too
    glGenTextures(4, depthTexture);
    
    initialize_colour_buffer(framebuffer[2], depthTexture[2], width, height); //screen texture will be written to this
    initialize_depth_buffer(framebuffer[0], depthTexture[0], width, height);  //outer light buffer (shadow map)
    initialize_depth_buffer(framebuffer[1], depthTexture[1], width, height);  //inner light buffer
    initialize_colour_buffer(framebuffer[3], depthTexture[3], width, height); //blurring texture
//-------------------------------------------------------------------------------------------------------------
//CREATE VERTEX AND INDEX BUFFERS:

    GLuint vertexbuffer[3];
    glGenBuffers(3, vertexbuffer);
    GLuint indexbuffer[2];
    glGenBuffers(2, indexbuffer);
    GLuint uvbuffer[3];
    glGenBuffers(3, uvbuffer);

    initialize_array_buffer(vertexbuffer[0], 3*number_of_vertices*sizeof(float), &Vx[0]); //buffers for initial screen data
    initialize_array_buffer(uvbuffer[0], 2*number_of_vertices*sizeof(float), &VT[0]);
    initialize_element_array_buffer(indexbuffer[0],3*number_of_faces*sizeof(unsigned int), &FV[0]);
    
    initialize_array_buffer(vertexbuffer[1], 3*number_of_vertices_puppet*sizeof(float), &Vx_puppet[0]); //buffers for puppet data
    initialize_array_buffer(uvbuffer[1], 2*number_of_vertices_puppet*sizeof(float), &VT_puppet[0]);
    initialize_element_array_buffer(indexbuffer[1],3*number_of_faces_puppet*sizeof(unsigned int), &FV_puppet[0]);

    initialize_array_buffer(vertexbuffer[2], sizeof(ver), ver); //buffers for final screen data
    initialize_array_buffer(uvbuffer[2], sizeof(col), col);
 
//---------------------------------------------------------------------------------------------------------------------------------------------------
//LOAD SHADERS:
    GLuint programID = LoadShaders("VertexShader.glsl", "FragmentShader.glsl"); //load screen shaders
    GLuint LightID = glGetUniformLocation(programID, "LightPos");
    GLuint texID= glGetUniformLocation(programID, "renderedTexture"); //three textures inputted to fragment shader
    GLuint depthID_inner = glGetUniformLocation(programID, "depthTexture"); //inner shadow map
    GLuint depthID_outer =  glGetUniformLocation(programID, "depthTexture_outer"); //outer shadow map
    glUseProgram(programID); 
    glUniform1i(texID, 0);
    glUniform1i(depthID_inner, 1);
    glUniform1i(depthID_outer, 2);

    GLuint depthprogramID = LoadShaders("VertexShader_fb.glsl", "FragmentShader_fb.glsl"); //load shaders
    GLuint depthMatrixID = glGetUniformLocation(depthprogramID, "depthMVP"); //load MVP matrix to shader
    GLuint rotationID = glGetUniformLocation( depthprogramID, "rotation"); //load in rotation matrix
    GLuint puppetID = glGetUniformLocation(depthprogramID, "puppet_texture");
    glUseProgram(depthprogramID); 
    glUniform1i(puppetID, 0);

    GLuint screenprogramID = LoadShaders("VertexShader_cb.glsl", "FragmentShader_cb.glsl"); //load shaders
    GLuint screen_LightID = glGetUniformLocation(screenprogramID, "LightPos"); //load light pos
    GLuint screenID = glGetUniformLocation(screenprogramID, "screenTexture");
    glUseProgram(screenprogramID); 
    glUniform1i(screenID, 0);

    GLuint blurringID =  LoadShaders("VertexShader_blur.glsl", "FragmentShader_blur.glsl"); //load blurring shader program
    GLuint widthID = glGetUniformLocation(programID, "widthRatio");
    GLuint heightID = glGetUniformLocation(programID, "heightRatio");
    GLuint depthMatrixIDb = glGetUniformLocation(blurringID, "depthMVP");
    GLuint puppetID2 = glGetUniformLocation(depthprogramID, "puppet_texture");
    glUseProgram(depthprogramID); 
    glUniform1i(puppetID2, 0);

    glUseProgram(screenprogramID); //create screen texture with lighting model
    GLint posAttribs = glGetAttribLocation(screenprogramID, "position");
    write_to_colour_buffer(framebuffer[2], textureID[0], vertexbuffer[0], indexbuffer[0], uvbuffer[0], posAttribs, number_of_faces, screen_LightID, LightPos);
    
    glUseProgram(blurringID); //created blurred inside of shadow
    GLint posAttribb = glGetAttribLocation(screenprogramID, "position");
    glUniform1f(heightID, heightRatio);
    glUniform1f(widthID, widthRatio);
    glUniformMatrix4fv(depthMatrixIDb, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);
    write_to_colour_buffer(framebuffer[3], textureID[1], vertexbuffer[0], indexbuffer[0], uvbuffer[0], posAttribb, number_of_faces, screen_LightID, LightPos);
   

    int  it=0; //number of iterations
    double start = glfwGetTime(); //start timer

    do{ //while window is open
        it++;
      
        glUseProgram(depthprogramID); //use shadow map shaders       
        GLint posAttrib_shadow = glGetAttribLocation(depthprogramID, "position");   //Render to shadow maps:
        write_to_shadow_map(framebuffer[1],depthMatrixID, depthMVP, vertexbuffer[1], posAttrib_shadow, number_of_faces_puppet,indexbuffer[1],rotationID, rotation, depthTexture[3], uvbuffer[1]); //pass blurred image to depth buffer
        write_to_shadow_map(framebuffer[0],depthMatrixID, depthMVP_outer, vertexbuffer[1], posAttrib_shadow, number_of_faces_puppet,indexbuffer[1],rotationID,rotation, textureID[1], uvbuffer[1]);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); //bind to default frame buffer
        glViewport(0,0,width,height); //fill whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID); //use screen shaders
        glActiveTexture(GL_TEXTURE0); //load in screen texture
        glBindTexture(GL_TEXTURE_2D, depthTexture[2]);
        glActiveTexture(GL_TEXTURE1); //load in shadow map
        glBindTexture(GL_TEXTURE_2D, depthTexture[1]);
        glActiveTexture(GL_TEXTURE2); //load in shadow map
        glBindTexture(GL_TEXTURE_2D, depthTexture[0]);
        glUniform3fv(LightID,1,&LightPos[0]);

        glEnableVertexAttribArray(0); //draw quad with textures mapped on.
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
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
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
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

        // rot_angle += 0.01f; 
        // rotation = {cos(rot_angle), sin(rot_angle), 0.0f, 0.0f,
        //             - sin(rot_angle), cos(rot_angle),0.0f, 0.0f,
        //             0.0f, 0.0f,1.0f, 0.0f,
        //             0.0f, 0.0f, 0.0f,1.0f,};
     

    }while(glfwGetKey(window, GLFW_KEY_ESCAPE)!=GLFW_PRESS && glfwWindowShouldClose(window)==0); //close if escape pressed
   
    double end = glfwGetTime();
    double elapsed_time = end - start;
    double a = elapsed_time/(double)it;
    std::cout<<" frame time "<<a<<" frame rate "<<1.0f/(a)<<"\n"; //show frame rate

//clear up and delete buffers
    delete[] texture_data;
    delete[] puppet_data;
    ObjFile::clean_up(Vx_puppet,N_puppet, VT_puppet, FV_puppet, FN_puppet, FT_puppet);
    ObjFile::clean_up(Vx,N, VT, FV, FN, FT);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteBuffers(3, vertexbuffer);
    glDeleteBuffers(3, uvbuffer);
    glDeleteBuffers(4, depthTexture);
    glDeleteBuffers(2, textureID);
    glDeleteFramebuffers(4, framebuffer);  

    return 0;
}