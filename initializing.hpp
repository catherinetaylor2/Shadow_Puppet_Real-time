#ifndef initializing_hpp
#define initializing_hpp

#include <glew.h>
#include <glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

void write_to_shadow_map(GLuint framebuffer,GLuint depthMatrixID, glm::mat4 depthMVP, GLuint puppet_vertexbuffer, GLint posAttrib, int number_of_faces_puppet, GLuint IBO, GLuint rotationID, glm::mat4 rotation, GLuint puppet_textureID, GLuint UVbuffer);
void initialize_depth_buffer(GLuint framebuffer, GLuint renderedTexture, int width, int height);
void initialize_colour_buffer(GLuint framebuffer, GLuint renderedTexture, int width, int height);
void initialize_texture(GLuint textureID, unsigned char* data, int width, int height);
void initialize_array_buffer(GLuint array_buffer, float size, float data []);
void initialize_element_array_buffer(GLuint array_buffer, int size, int data []);
void write_to_colour_buffer(GLuint framebuffer, GLuint textureID, GLuint vertexbuffer, GLuint indexbuffer, GLuint uvbuffer, GLint posAttrib, int number_of_faces, GLuint LightID, glm::vec3 LightPos);

#endif