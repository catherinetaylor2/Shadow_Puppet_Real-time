#ifndef openGLCalculations_hpp
#define openGLCalculations_hpp

#include <glew.h>
#include <glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

void write_to_shadow_map(GLuint framebuffer, GLuint MVPMatrixID, glm::mat4 MVP, GLuint vertexbuffer, GLint posAttrib, int NumberOfFaces, GLuint IndexBuffer, GLuint rotationMatrixID, glm::mat4 rotationMatrix, GLuint textureID, GLuint UVbuffer);
void initialize_colour_buffer(GLuint framebuffer, GLuint renderedTexture, int width, int height, int i, bool IsFloat);
void initialize_texture(GLuint textureID, unsigned char* data, int width, int height);
void initialize_array_buffer(GLuint arrayBuffer, float size, float data []);
void initialize_element_array_buffer(GLuint arrayBuffer, int size, int data []);
void write_to_colour_buffer(GLuint framebuffer, GLuint textureID, GLuint vertexbuffer, GLuint indexbuffer, GLuint uvbuffer, GLint posAttrib, int numberOfFaces, GLuint LightID, glm::vec3 LightPos);
glm::mat4 GetLightCornerMatrix(float LightLength, glm::vec3 LightCentre);
void DrawScreenQuad(GLuint vertexbuffer, GLuint uvbuffer, GLuint programID );

#endif