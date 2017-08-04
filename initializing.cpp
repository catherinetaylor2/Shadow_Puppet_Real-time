//contains code to set up buffers

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "initializing.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

void write_to_shadow_map(GLuint framebuffer,GLuint MVPMatrixID, glm::mat4 MVPMatrix, GLuint vertexBuffer, GLint posAttrib, int NumberOfFaces, GLuint IndexBuffer, GLuint RotationMatrixMatrixID, glm::mat4 RotationMatrix, GLuint textureID, GLuint UVbuffer){
  
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); //bind to shadow map
    glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &MVPMatrix[0][0]); //load in MVP matrix
    glUniformMatrix4fv(RotationMatrixMatrixID, 1, GL_FALSE, &RotationMatrix[0][0]); //Load in rotation matrix

    glActiveTexture(GL_TEXTURE0); //load in textures
    glBindTexture(GL_TEXTURE_2D, textureID);

    glEnable(GL_DEPTH_TEST); //find depth values
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer); //bind to index buffer
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

    glDrawElements(GL_TRIANGLES, 3*NumberOfFaces,  GL_UNSIGNED_INT,0); //render to depth buffer
    glDisableVertexAttribArray(0);
}

void initialize_colour_buffer(GLuint framebuffer, GLuint renderedTexture, int width, int height, int i, bool isFloat){

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindTexture(GL_TEXTURE_2D, renderedTexture); //set up inner shadow map
    if(isFloat == true){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    }
    else{
        if (i==0){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }
        else{
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        }
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float C [3] = {0.0,0.0,0.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, C);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0); 
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "Error: frame buffer not complete \n" ; //check depth buffer is complete
    } 

}

void initialize_texture(GLuint textureID, unsigned char* data, int width, int height){
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void initialize_array_buffer(GLuint array_buffer, float size, float data []){
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer); //buffer containing screen mesh
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void initialize_element_array_buffer(GLuint array_buffer, int size, int data []){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, array_buffer); //buffer containing screen mesh
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void write_to_colour_buffer(GLuint framebuffer, GLuint textureID, GLuint vertexbuffer, GLuint indexbuffer, GLuint uvbuffer, GLint posAttrib, int NumberOfFaces, GLuint LightID, glm::vec3 LightPos){
   
    glActiveTexture(GL_TEXTURE0); //load in screen texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glUniform3fv(LightID,1,&LightPos[0]);

    glEnableVertexAttribArray(0); //draw quad with textures mapped on.
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer); 
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
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        0,  
        (void*)0
    );
    glDrawElements(GL_TRIANGLES, 3*NumberOfFaces,  GL_UNSIGNED_INT,0); // draw mesh
    glDisableVertexAttribArray(0);
}


glm::mat4 GetLightCornerMatrix(float LightLength, glm::vec3 LightPos){
    glm::mat4 LightCorners ={LightPos.x - LightLength/2.0f, LightPos.y + LightLength/2.0f, LightPos.z, 0,
                            LightPos.x + LightLength/2.0f, LightPos.y + LightLength/2.0f, LightPos.z, 0,
                            LightPos.x - LightLength/2.0f, LightPos.y - LightLength/2.0f, LightPos.z,0,
                            LightPos.x + LightLength/2.0f, LightPos.y - LightLength/2.0f, LightPos.z, 0};
    return LightCorners;
}

void DrawScreenQuad(GLuint vertexbuffer, GLuint uvbuffer, GLuint programID ){
    glEnableVertexAttribArray(0); //draw quad with textures mapped on.
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    int posAttrib = glGetAttribLocation(programID, "position");
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
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
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
}
