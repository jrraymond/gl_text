#ifndef TEXT2D_H
#define TEXT2D_H
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fts.h>
#include <GL/glew.h>
#include "shader_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Text {
private:
    GLuint frag_shader ;
    GLuint vert_shader ;
    GLuint program ;
    GLint pos_attr ;
    GLint tex_unif ;
    GLint clr_unif ;

    struct point {
        GLfloat x ;
        GLfloat y ;
        GLfloat s ;
        GLfloat t ;
    } ;

    GLuint vbo ;
    GLuint vao ;

    FT_Library ft ;
    FT_Face face ;

    const char* fontfilename ;
    static bool initialized ;
public:
    Text(std::string font_f_name) ;

    void render_text(const char *text, float x, float y, float sx, float sy) ;

    void display(int w, int h);

    ~Text();
} ;

#endif
