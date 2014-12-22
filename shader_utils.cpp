#include "shader_utils.h"

void print_shader_comp_info(GLuint shader_index) {
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetShaderInfoLog (shader_index, max_length, &actual_length, log);
    fprintf(stderr, "shader info log for GL index %i:\n%s\n", shader_index, log);
}
void print_shader_link_info(GLuint sp) {
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetProgramInfoLog (sp, max_length, &actual_length, log);
    fprintf(stderr, "program info log for GL index %i:\n%s", sp, log);
}
bool shader_is_valid(GLuint sp) {
    int params = -1;
    glValidateProgram (sp);
    glGetProgramiv (sp, GL_VALIDATE_STATUS, &params);
    fprintf(stdout,"program %i GL_VALIDATE_STATUS = %i\n", sp, params);
    if (GL_TRUE != params) {
        print_shader_link_info(sp);
        return false;
    }
    return true;
}
bool read_file(const char* f_name, char* str, int max_len) {
    FILE* fp = fopen(f_name , "r") ;
    int len = 0 ;
    char line[80] ;

    str[0] = '\0' ;
    if (fp == NULL) {
        fprintf(stderr, "ERROR: error opening file: %s\n", f_name) ;
        return false ;
    }
    strcpy(line, "") ;
    while (!feof(fp)) {
        if (NULL != fgets(line, 80, fp)) {
            len += strlen(line) ;
            if (len >= max_len) {
                fprintf(stderr, "ERROR: file length greater than max length: %i", max_len ) ;
            }
            strcat(str, line) ;
        }
    }
    if (fclose(fp) == EOF) {
        fprintf(stderr, "ERROR: closing file %s\n", f_name) ;
        return false ;
    }
    return true ;
}
/* compiles a shader given file name containing source and shader index */
bool compile_shader(const char* f_name, GLuint shader) {
    GLint status ;
    char src[MAX_SHADER_SRC_SIZE] ;
    if (!read_file(f_name, src, MAX_SHADER_SRC_SIZE)) {  return false ;  }
    const GLchar* glsrc = (const GLchar*) src ;
    glShaderSource(shader, 1, &glsrc, NULL) ;
    glCompileShader(shader) ;
    //check for compilation errors
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status) ;
    if (GL_TRUE != status) {
        fprintf(stderr, "ERROR: GL shader %s index %i did not compile\n", f_name, shader) ;
        print_shader_comp_info(shader) ;
        return false ;
    }
    return true ;
}
bool link_shader(GLuint prog, std::initializer_list<GLuint> shaders) {
    GLint status ;
    for(auto x : shaders) {
        glAttachShader(prog, x) ;
    }
    glLinkProgram(prog) ;
    glGetProgramiv(prog, GL_LINK_STATUS, &status) ;
    if (GL_TRUE != status) {
        fprintf(
                stderr,
                "ERROR: could not link shader programme GL index %i\n",
                prog
        );
        print_shader_link_info(prog) ;
        return false ;
    }
    return true ;
}
void check_gl_errors(std::string s) {
    GLenum gl_err = glGetError();
    if (GL_NO_ERROR != gl_err) {
        fprintf(stderr, "%s: gl error: %i, %s\n", s.c_str(), gl_err, gluErrorString(gl_err));
    }
    else {
        fprintf(stderr, "%s: no gl error\n", s.c_str()) ;
    }
}
