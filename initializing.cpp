//contains code to set up buffers

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "initializing.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

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
void initialize_depth_buffer(GLuint framebuffer, GLuint renderedTexture, int width, int height){

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindTexture(GL_TEXTURE_2D, renderedTexture); //set up inner shadow map
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderedTexture, 0);  
    glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "Error: frame buffer not complete \n" ;
    } //check depth buffer is complete
       
};

void initialize_colour_buffer(GLuint framebuffer, GLuint renderedTexture, int width, int height){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindTexture(GL_TEXTURE_2D, renderedTexture); //set up inner shadow map
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0); 
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "Error: frame buffer not complete \n" ;
    } //check depth buffer is complete

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
void write_to_colour_buffer(GLuint framebuffer, GLuint textureID, GLuint vertexbuffer, GLuint indexbuffer, GLuint uvbuffer, GLint posAttrib, int number_of_faces, GLuint LightID, glm::vec3 LightPos){
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
    glDrawElements(GL_TRIANGLES, 3*number_of_faces,  GL_UNSIGNED_INT,0); // draw mesh
    glDisableVertexAttribArray(0);
}
