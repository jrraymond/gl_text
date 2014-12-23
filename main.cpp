#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader_utils.h"
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
    Text text ;
    if (text.Text::init_resources("FreeSans.ttf") == 0) {
        return -1;
    }
    while (!glfwWindowShouldClose(window)) {
        text.Text::display(window_w, window_h);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
