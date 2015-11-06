#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader_utils/shader_utils.h"
#include "text.h"

int main() {
    if (glfwInit() != GL_TRUE) {
        fprintf(stderr, "Failed to initialize GLFW\n") ;
        return -1 ;
    }
    check_gl_errors("glfw init") ;
    // Create a rendering window with OpenGL 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3) ;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2) ;
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE) ;
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE) ;
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE) ;

    int window_h = 600, window_w = 800 ;
    GLFWwindow* window = glfwCreateWindow(window_w, window_h, "OpenGL", NULL, NULL) ;
    glfwMakeContextCurrent(window) ;
    check_gl_errors("glfw window context") ;
    glewExperimental = GL_TRUE ;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n") ;
        return -1 ;
    }
    check_gl_errors("glew set up") ;
    Text text ("OpenSans-Regular.ttf") ;
    while (!glfwWindowShouldClose(window)) {
        GLfloat black[4] = { 1, 1, 1, 1 } ;
        text.Text::display("TESTING testing 1 1 12", -1, 0.9, 1.0 / window_w, 1.0 / window_h, black, 12);
        text.Text::display("TESTING testing 1 1 48", -1, 0.8, 1.0 / window_w, 1.0 / window_h, black, 48);
        text.Text::display("TESTING testing 2 2 48", -1, 0.7, 2.0 / window_w, 2.0 / window_h, black, 48);
        //text.Text::render("TESTING TESTING TESTIGN",1,1, 2/window_w,2/window_h, black, 48) ;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
