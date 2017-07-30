// Real-time Shadow Puppet Theatre
//
// Created by Catherine Taylor
//
//Began June 2017

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include "shader.hpp"
#include "BITMAP.hpp"
#include"Read_Obj.hpp"
#include <vector>
#include "initializing.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

void CreateIntegralImage(unsigned char *InputImage, int width, int height, float **IntegralImage){
    (*IntegralImage) = new float [3*width*height];
    float value = 0;
    for(int i=width-1; i>-1; --i){
        value += ((float)InputImage[3*i] ==0);
        (*IntegralImage)[3*i+(height-1)*width*3] = value;
        (*IntegralImage)[3*i+1+(height-1)*width*3] = value;
        (*IntegralImage)[3*i+2+(height-1)*width*3] = value;
    }
    value = 0;
    for(int j=height-1; j>-1; --j){
        value += ((float)InputImage[j*width*3] ==0);
        (*IntegralImage)[j*width*3+3*(width-1)] = value;
        (*IntegralImage)[j*width*3+1+3*(width-1)] = value;
        (*IntegralImage)[j*width*3+2+3*(width-1)] = value;
    }

    
    for(int j = height-2; j>0; --j){
        value = 0.0f;
        for(int i=1; i<width; ++i){
            float temp = (float)(*IntegralImage)[(j+1)*width*3 + 3*i];
           
            value = ((float)InputImage[j*width*3+3*i] ==0) + (float)(*IntegralImage)[(j)*width*3 + 3*(i-1)] + temp -  (float)(*IntegralImage)[(j+1)*width*3 + 3*(i-1)];
            (*IntegralImage)[j*width*3+3*i] = value;
            (*IntegralImage)[j*width*3+3*i+1] = 0;// value;
            (*IntegralImage)[j*width*3+3*i+2] =  0;//value;
        }
    }
}

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

    float* IntegralImage;

    unsigned char * ScreenTextureData, *PuppetTextureData; 
	int texture_width, texture_height, PuppetWidth, PuppetHeight;
	ScreenTextureData = readBMP("Textures/sheet.bmp", &texture_width, &texture_height); //screen texture data
    PuppetTextureData = readBMP("Textures/dino_texture.bmp", &PuppetWidth, &PuppetHeight); //puppet texture data

    CreateIntegralImage(PuppetTextureData, PuppetWidth, PuppetHeight, &IntegralImage);

    float *VerticesPuppet, *NormalsPuppet, *TexturesPuppet, *VerticesScreen, *NormalsScreen, *TexturesScreen; 
    int NumberOfPuppetFaces, *FaceVerticesPuppet, *FaceNormalsPuppet, *FaceTexturesPuppet, NumberOfPuppetVertices, NumberOfScreenFaces, *FaceVerticesScreen, *FaceNormalsScreen, *FaceTexturesScreen, NumberOfScreenVertices;
    ObjFile mesh_puppet("Objects/quad2.obj"); //Input mesh of puppet as obj file
	mesh_puppet.get_mesh_data(mesh_puppet, &FaceVerticesPuppet, &FaceNormalsPuppet, &FaceTexturesPuppet, &TexturesPuppet, &NormalsPuppet, &VerticesPuppet, &NumberOfPuppetFaces, &NumberOfPuppetVertices);
    ObjFile mesh_screen("Objects/plane.obj"); //Input plane with lots of triangles
	mesh_screen.get_mesh_data(mesh_screen, &FaceVerticesScreen, &FaceNormalsScreen, &FaceTexturesScreen, &TexturesScreen, &NormalsScreen, &VerticesScreen, &NumberOfScreenFaces, &NumberOfScreenVertices);

	std::cout<<"Inputed files loaded \n"; //ADD IN ERROR TEST?

    if(!glfwInit()){ // initialize GLFW
        std::cerr<<"Error: failed to initialize GLFW \n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES,4); // select openGL version settings 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window; //create openGL window
    window = glfwCreateWindow(width, height, "Shadow Puppet", NULL, NULL);
    
    if(window==NULL){
        std::cerr<<"Error: failed to open window";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if(glewInit()!=GLEW_OK){
        std::cerr<<"Error: failed to initialize GLEW \n";
        return -1;
    }

    GLuint VertexArrayID; //Create vertex array object and set to be current
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); //set keys for window

//INITIAL POSITION DATA-----------------------------------------------------------------------------------------
   
//light data
    glm::vec3 LightPos = glm::vec3(0.0f,0.0f,50.0f);
    float LightLength = 1.75f;
    glm::mat4 LightCorners =GetLightCornerMatrix(LightLength, LightPos);

    glm::vec3 LightPosOuter = glm::vec3(0.0f,0.0f,30.0f);
    float LightLengthOuter = 0.05f;
    glm::mat4 LightCornersOuter =GetLightCornerMatrix(LightLengthOuter, LightPosOuter);

    glm::vec3 LightInvDir = glm::vec3(0.0f, 0, -10); //find objects which occlude the light source
    glm::mat4 depthProjMatrix = glm::ortho<float>(-0.6,0.6,-0.6,0.6,-10,12); //size of dino visible from inner light
    glm::mat4 depthViewMatrix = glm::lookAt(LightInvDir, glm::vec3(0,0,-1), glm::vec3(0,1,0));
    glm::mat4 depthMVP = depthProjMatrix*depthViewMatrix;
    glm::mat4 depthProjMatrixOuter = glm::ortho<float>(-0.55,0.55,-0.55,0.55,-10,15); //size of dino visible from outer light
    glm::mat4 depthMVPOuter = depthProjMatrixOuter*depthViewMatrix;

    float RotAngle = 0.0f; //set up initial rotation matrix
    glm::mat4 rotation = {  cos(RotAngle), sin(RotAngle), 0.0f, 0.0f,
                            - sin(RotAngle), cos(RotAngle),0.0f, 0.0f,
                            0.0f, 0.0f,1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f,1.0f,};

    float ScreenVer [] = { //quad filling whole screen with only 2 triangles
        -1.0f,-1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f,1.0f, 0.0f,
    };

    float ScreenTex [] = { //UV coords
        0.0f, 0.0f, 
        1.0f, 0.0f, 
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f, 
        0.0f, 1.0f,
    };
    glm::mat4 PuppetCorners = {VerticesPuppet[6],VerticesPuppet[7],VerticesPuppet[8], 0.0f,
    VerticesPuppet[9],VerticesPuppet[10],VerticesPuppet[11],0.0f,
    VerticesPuppet[0],VerticesPuppet[1],VerticesPuppet[2], 0.0f,
    NormalsPuppet[0],NormalsPuppet[1],NormalsPuppet[2],1.0f, //puppet normal
    };
//-----------------------------------------------------------------------------------------------------------
//CREATE FRAMEBUFFERS AND TEXTURES: 

    GLuint textureID[2]; //create texture from inputted bitmaps
    glGenTextures(2, textureID);
    initialize_texture(textureID[0], ScreenTextureData, texture_width, texture_height);
    initialize_Integral_texture(textureID[1], IntegralImage, PuppetWidth, PuppetHeight);

    GLuint framebuffer[3]; //create 3 framebuffers
    glGenFramebuffers(3, framebuffer);
    GLuint depthTexture[3]; // generate 3 textures which will be written too
    glGenTextures(3, depthTexture);
    
    initialize_colour_buffer(framebuffer[2], depthTexture[2], width, height); //screen texture will be written to this
    initialize_colour_buffer(framebuffer[0], depthTexture[0], width, height);  //outer light buffer (shadow map)
    initialize_colour_buffer(framebuffer[1], depthTexture[1], width, height); //inner light buffer
//-------------------------------------------------------------------------------------------------------------
//CREATE VERTEX AND INDEX BUFFERS:

    GLuint vertexbuffer[3];
    glGenBuffers(3, vertexbuffer);
    GLuint indexbuffer[2];
    glGenBuffers(2, indexbuffer);
    GLuint uvbuffer[3];
    glGenBuffers(3, uvbuffer);

    initialize_array_buffer(vertexbuffer[0], 3*NumberOfScreenVertices*sizeof(float), &VerticesScreen[0]); //buffers for initial screen data
    initialize_array_buffer(uvbuffer[0], 2*NumberOfScreenVertices*sizeof(float), &TexturesScreen[0]);
    initialize_element_array_buffer(indexbuffer[0],3*NumberOfScreenFaces*sizeof(unsigned int), &FaceVerticesScreen[0]);
    
    initialize_array_buffer(vertexbuffer[1], 3*NumberOfPuppetVertices*sizeof(float), &VerticesPuppet[0]); //buffers for puppet data
    initialize_array_buffer(uvbuffer[1], 2*NumberOfPuppetVertices*sizeof(float), &TexturesPuppet[0]);
    initialize_element_array_buffer(indexbuffer[1],3*NumberOfPuppetFaces*sizeof(unsigned int), &FaceVerticesPuppet[0]);

    initialize_array_buffer(vertexbuffer[2], sizeof(ScreenVer), ScreenVer); //buffers for final screen data
    initialize_array_buffer(uvbuffer[2], sizeof(ScreenTex),ScreenTex);
 
//---------------------------------------------------------------------------------------------------------------------------------------------------
//LOAD SHADERS:
    GLuint programID = LoadShaders("Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl"); //load screen shaders
    GLuint LightID = glGetUniformLocation(programID, "LightPos");
    GLuint ScreenTextureID = glGetUniformLocation(programID, "renderedTexture"); //three textures inputted to fragment shader
    GLuint ShadowMapInnerID = glGetUniformLocation(programID, "depthTexture"); //inner shadow map
    GLuint ShadowMapOuterID =  glGetUniformLocation(programID, "depthTexture_outer"); //outer shadow map
    glUseProgram(programID); 
    glUniform1i(ScreenTextureID, 0);
    glUniform1i(ShadowMapInnerID, 1);
    glUniform1i(ShadowMapOuterID, 2);

    // GLuint ShadowMapProgramID = LoadShaders("Shaders/VertexShader_fb.glsl", "Shaders/FragmentShader_fb.glsl"); //load shaders
    // GLuint depthMatrixID = glGetUniformLocation(ShadowMapProgramID, "depthMVP"); //load MVP matrix to shader
    // GLuint rotationMatrixID = glGetUniformLocation(ShadowMapProgramID, "rotation"); //load in rotation matrix
    // GLuint PuppetTextureID = glGetUniformLocation(ShadowMapProgramID, "puppet_texture");
    // glUseProgram(ShadowMapProgramID); 
    // glUniform1i(PuppetTextureID, 0);

    GLuint screenprogramID = LoadShaders("Shaders/VertexShaderScreenTex.glsl", "Shaders/FragmentShaderScreenTex.glsl"); //load shaders
    GLuint screen_LightID = glGetUniformLocation(screenprogramID, "LightPos"); //load light pos
    GLuint screenID = glGetUniformLocation(screenprogramID, "screenTexture");
    glUseProgram(screenprogramID); 
    glUniform1i(screenID, 0);

    GLuint BlurringProgramID =  LoadShaders("Shaders/VertexShaderVisFun.glsl", "Shaders/FragmentShaderVisFun.glsl"); //load blurring shader program
    GLuint widthID = glGetUniformLocation(BlurringProgramID, "textureXres");
    GLuint heightID = glGetUniformLocation(BlurringProgramID, "textureYres");
    GLuint CornerID = glGetUniformLocation(BlurringProgramID, "Corners");
    GLuint PuppetCornerID = glGetUniformLocation(BlurringProgramID, "PuppetCorners");
    GLuint blurringPuppetTextureID = glGetUniformLocation(BlurringProgramID, "puppet_texture");
    glUseProgram(BlurringProgramID); 
    glUniform1i(blurringPuppetTextureID, 0);

    glUseProgram(screenprogramID); //create screen texture with lighting model
    GLint posAttribs = glGetAttribLocation(screenprogramID, "position");
    write_to_colour_buffer(framebuffer[2], textureID[0], vertexbuffer[0], indexbuffer[0], uvbuffer[0], posAttribs, NumberOfScreenFaces, screen_LightID, LightPos);
    
 
    int  iterations = 0; //number of iterations
    double StartTime = glfwGetTime(); //Start timer

    do{ //while window is open
        iterations++;

    glUseProgram(BlurringProgramID); //created blurred inside of shadow
    GLint posAttribb = glGetAttribLocation(BlurringProgramID, "position");
    glUniform1f(heightID, PuppetHeight);
    glUniform1f(widthID, PuppetWidth);
    glUniformMatrix4fv(CornerID, 1, GL_FALSE, &LightCorners[0][0]);
    glUniformMatrix4fv(PuppetCornerID, 1, GL_FALSE, &PuppetCorners[0][0]);
    write_to_colour_buffer(framebuffer[1], textureID[1], vertexbuffer[0], indexbuffer[0], uvbuffer[0], posAttribb, NumberOfScreenFaces, screen_LightID, LightPos);
    glUniformMatrix4fv(CornerID, 1, GL_FALSE, &LightCornersOuter[0][0]);
    write_to_colour_buffer(framebuffer[0], textureID[1], vertexbuffer[0], indexbuffer[0], uvbuffer[0], posAttribb, NumberOfScreenFaces, screen_LightID, LightPos);

      
    //     glUseProgram(ShadowMapProgramID); //use shadow map shaders       
    //     GLint posAttrib_shadow = glGetAttribLocation(ShadowMapProgramID, "position");   //REndTimeer to shadow maps:
    //    write_to_shadow_map(framebuffer[1],depthMatrixID, depthMVP, vertexbuffer[1], posAttrib_shadow, NumberOfPuppetFaces,indexbuffer[1],rotationMatrixID, rotation, textureID[1], uvbuffer[1]); //pass blurred image to depth buffer
    //    write_to_shadow_map(framebuffer[0],depthMatrixID, depthMVPOuter, vertexbuffer[1], posAttrib_shadow, NumberOfPuppetFaces,indexbuffer[1],rotationMatrixID,rotation, textureID[1], uvbuffer[1]);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); //bind to default frame buffer
        glViewport(0,0,width,height); //fill whole screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID); //use screen shaders
        glActiveTexture(GL_TEXTURE0); //load in screen texture
        glBindTexture(GL_TEXTURE_2D, depthTexture[2]);
        glActiveTexture(GL_TEXTURE1); //load in inner shadow map
        glBindTexture(GL_TEXTURE_2D, depthTexture[1]);
        glActiveTexture(GL_TEXTURE2); //load in outer shadow map
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

        // RotAngle += 0.01f; 
        // rotation = {cos(RotAngle), sin(RotAngle), 0.0f, 0.0f,
        //             - sin(RotAngle), cos(RotAngle),0.0f, 0.0f,
        //             0.0f, 0.0f,1.0f, 0.0f,
        //             0.0f, 0.0f, 0.0f,1.0f,};
     

    }while(glfwGetKey(window, GLFW_KEY_ESCAPE)!=GLFW_PRESS && glfwWindowShouldClose(window)==0); //close if escape pressed
   
    double EndTime = glfwGetTime();
    double ElapsedTime = EndTime - StartTime;
    double frameTime = ElapsedTime/(double)iterations;
    std::cout<<" frame time "<<frameTime<<" frame rate "<<1.0f/frameTime<<"\n"; //show frame rate

//clear up and delete buffers
    delete[] ScreenTextureData;
    delete[] PuppetTextureData;
    delete[] IntegralImage;
    ObjFile::clean_up(VerticesPuppet,NormalsPuppet, TexturesPuppet, FaceVerticesPuppet, FaceNormalsPuppet, FaceTexturesPuppet);
    ObjFile::clean_up(VerticesScreen,NormalsScreen, TexturesScreen, FaceVerticesScreen, FaceNormalsScreen, FaceTexturesScreen);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteBuffers(3, vertexbuffer);
    glDeleteBuffers(3, uvbuffer);
    glDeleteBuffers(3, depthTexture);
    glDeleteBuffers(2, textureID);
    glDeleteFramebuffers(3, framebuffer);  

    return 0;
}