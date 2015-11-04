#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(void) {

    GLuint WIDTH, HEIGHT;

    WIDTH = 800;
    HEIGHT = 600;

    GLFWwindow* window;

    if(!glfwInit()) {
        fprintf(stderr, "Error: Could not load GLFW, aborting.\n");
        return(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "01_shaders", NULL, NULL);
    if(!window) {
        fprintf(stderr, "Error: Could not create window, aborting.\n");
        return(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    printf("Hello.\n");
    return 0;
}
