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
    GLuint program;
    GLint attribute_coord;
    GLint uniform_tex;
    GLint uniform_color;

    struct point {
        GLfloat x;
        GLfloat y;
        GLfloat s;
        GLfloat t;
    };

    GLuint vbo;

    FT_Library ft;
    FT_Face face;

    const char *fontfilename;
public:
    void check_gl_errors() {
        GLenum gl_err = glGetError();
        if (GL_NO_ERROR != gl_err) {
            fprintf(stderr, "gl error: %i, \n", gl_err);
        }
        else {
            fprintf(stderr, "no gl error\n") ;
        }
    }

    int init_resources(std::string f_name) {
        /* Initialize the FreeType2 library */
        if (FT_Init_FreeType(&ft)) {
            fprintf(stderr, "Could not init freetype library\n");
            return 0;
        }

        /* Load a font */
        if (FT_New_Face(ft, f_name.c_str(), 0, &face)) {
            fprintf(stderr, "Could not open font %s\n", f_name.c_str());
            return 0;
        }
        vert_shader = glCreateShader(GL_VERTEX_SHADER);
        compile_shader("vertex_shader.glsl", vert_shader) ;
        frag_shader = glCreateShader(GL_FRAGMENT_SHADER) ;
        compile_shader("fragement_shader.glsl", frag_shader) ;
        program = glCreateProgram() ;
        link_shader(program, {vert_shader, frag_shader}) ;
        if(program == 0)
            return 0;

        attribute_coord = glGetAttribLocation(program, "coord");
        uniform_tex = glGetUniformLocation(program, "tex");
        uniform_color = glGetAttribLocation(program, "color");

        if(attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
            return 0;

            // Create the vertex buffer object
        glGenBuffers(1, &vbo);

        return 1;
    }

/**
* Render text using the currently loaded font and currently set font size.
* Rendering starts at coordinates (x, y), z is always 0.
* The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
*/
    void render_text(const char *text, float x, float y, float sx, float sy) {
        const char *p;
        FT_GlyphSlot g = face->glyph;

        /* Create a texture that will be used to hold one "glyph" */
        GLuint tex;

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(uniform_tex, 0);

        /* We require 1 byte alignment when uploading texture data */
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        /* Clamping to edges is important to prevent artifacts when scaling */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        /* Linear filtering usually looks best for text */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        /* Set up the VBO for our vertex data */
        glEnableVertexAttribArray(attribute_coord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

        /* Loop through all characters */
        for (p = text; *p; p++) {
            /* Try to load and render the character */
            if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
                continue;

            /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

            /* Calculate the vertex and texture coordinates */
            float x2 = x + g->bitmap_left * sx;
            float y2 = -y - g->bitmap_top * sy;
            float w = g->bitmap.width * sx;
            float h = g->bitmap.rows * sy;

            point box[4] = {
                    {x2, -y2, 0, 0},
                    {x2 + w, -y2, 1, 0},
                    {x2, -y2 - h, 0, 1},
                    {x2 + w, -y2 - h, 1, 1},
            };

            /* Draw the character on the screen */
            glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            /* Advance the cursor to the start of the next character */
            x += (g->advance.x >> 6) * sx;
            y += (g->advance.y >> 6) * sy;
        }

        glDisableVertexAttribArray(attribute_coord);
        glDeleteTextures(1, &tex);
    }

    void display(int w, int h) {
        float sx = 2.0 / w ;
        float sy = 2.0 / h ;

        glUseProgram(program);

        /* White background */
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Enable blending, necessary for our alpha texture */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLfloat black[4] = { 0, 0, 0, 1 };
        GLfloat transparent_green[4] = { 0, 1, 0, 0.5 };

        /* Set font size to 48 pixels, color to black */
        FT_Set_Pixel_Sizes(face, 0, 48);
        glUniform4fv(uniform_color, 1, black);

        /* Effects of alignment */
        render_text("The Quick Brown Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 50 * sy, sx, sy);

        /* Scaling the texture versus changing the font size */
        render_text("The Small Texture Scaled Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 175 * sy, sx * 0.5, sy * 0.5);

        /* Colors and transparency */
        glUniform4fv(uniform_color, 1, transparent_green);
        render_text("The Transparent Green Fox Jumps Over The Lazy Dog", -1 + 18 * sx, 1 - 440 * sy, sx, sy);

    }

    void free_resources() {
        glDeleteProgram(program);
    }
};
#endif
