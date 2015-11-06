#include "text.h"

bool Text::initialized = false ;


Text::Text(std::string f_name) {
    if (Text::initialized) {
        fprintf(stderr, "Text already initialized\n") ;
        return ;
    }
    /* Initialize the FreeType2 library */
    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetype library\n") ;
        return ;
    }

    /* Load a font */
    if (FT_New_Face(ft, f_name.c_str(), 0, &face)) {
        fprintf(stderr, "Could not open font %s\n", f_name.c_str()) ;
        return ;
    }
    vert_shader = glCreateShader(GL_VERTEX_SHADER) ;
    compile_shader("vertex_shader.glsl", vert_shader) ;
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER) ;
    compile_shader("fragment_shader.glsl", frag_shader) ;
    program = glCreateProgram() ;
    link_shader(program, {vert_shader, frag_shader}) ;
    if(program == 0) {
        fprintf(stderr, "Could not open program\n") ;
        return ;
    }
    check_gl_errors("shaders") ;
    pos_attr = glGetAttribLocation(program, "pos") ;
    tex_unif = glGetUniformLocation(program, "texture") ;
    clr_unif = glGetUniformLocation(program, "color") ;
    check_gl_errors("locations") ;
    if(pos_attr == -1 || tex_unif == -1 || clr_unif == -1) {
        fprintf(stderr, "Could not get locations: %i\t%i\t%i\n", pos_attr, tex_unif, clr_unif) ;
        return ;
    }

    // Create the vertex buffer object
    glGenVertexArrays(1, &vao) ;
    glBindVertexArray(vao) ;
    glGenBuffers(1, &vbo) ;
    Text::initialized = true ;
}
Text::~Text() {
    glDeleteProgram(program) ;
    glDeleteVertexArrays(1, &vao) ;
    glDeleteBuffers(1, &vbo) ;
    FT_Done_Face(face) ;
    FT_Done_FreeType(ft) ;
    Text::initialized = false ;
}
/* Render text to the screen
 * @param txt the text to display
 * @param x the x coordinate (-1 to 1)
 * @param y the y coordinate (-1 to 1)
 * @param sx the x-scaling
 * @param sy the y-scaling
 * @param color the color of the text
 * @param font_sz the font size
 */
void Text::display(const char* txt, float x, float y, float sx, float sy, const GLfloat* color, int font_sz) {
    glUseProgram(program) ;

    /* Enable blending, necessary for our alpha texture */
    glEnable(GL_BLEND) ;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;

    /* Set font size to 48 pixels, color to black */
    FT_Set_Pixel_Sizes(face, 0, font_sz) ;
    glUniform4fv(clr_unif, 1, color) ;

    /* Effects of alignment */
    render_text(txt, x, y, sx, sy) ;
}

void Text::render_text(const char *text, float x, float y, float sx, float sy) {
    const char *p;
    FT_GlyphSlot g = face->glyph;
    glBindVertexArray(vao) ;
    /* Create a texture that will be used to hold one "glyph" */
    GLuint tex;

    glActiveTexture(GL_TEXTURE0) ;
    glGenTextures(1, &tex) ;
    glBindTexture(GL_TEXTURE_2D, tex) ;
    glUniform1i(tex_unif, 0) ;

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1) ;

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) ;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) ;

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ;

    /* Set up the VBO for our vertex data */
    glEnableVertexAttribArray(pos_attr) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo) ;
    glVertexAttribPointer(pos_attr, 4, GL_FLOAT, GL_FALSE, 0, 0) ;

    /* Loop through all characters */
    for (p = text; *p; p++) {
        /* Try to load and render the character */
        if (FT_Load_Char(face, *p, FT_LOAD_RENDER)) { continue ; }

        /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                g->bitmap.width,
                g->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                g->bitmap.buffer) ;

        /* Calculate the vertex and texture coordinates */
        float x2 = x + g->bitmap_left * sx ;
        float y2 = -y - g->bitmap_top * sy ;
        float w = g->bitmap.width * sx ;
        float h = g->bitmap.rows * sy ;

        point box[4] = {
                {x2, -y2, 0, 0},
                {x2 + w, -y2, 1, 0},
                {x2, -y2 - h, 0, 1},
                {x2 + w, -y2 - h, 1, 1},
        } ;

        /* Draw the character on the screen */
        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW) ;
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4) ;

        /* Advance the cursor to the start of the next character */
        x += (g->advance.x >> 6) * sx;
        y += (g->advance.y >> 6) * sy;
    }

    glDisableVertexAttribArray(pos_attr) ;
    glDeleteTextures(1, &tex) ;
}

