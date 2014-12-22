#include <GL/glew.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <initializer_list>

#define MAX_SHADER_SRC_SIZE 4096

bool shader_is_valid(GLuint sp) ;
void print_shader_link_info(GLuint sp) ;
void print_shader_comp_info(GLuint shader_index) ;
bool read_file(const char* f_name, char* str, int max_len) ;
bool compile_shader(const char* f_name, GLuint shader) ;
bool link_shader(GLuint prog, std::initializer_list<GLuint> shaders) ;
void check_gl_errors(std::string s) ;
