#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "file_utils.h"

#define UNUSED(x) (void)x

int main(void) {

    GLFWwindow *window;

    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW, aborting.\n");
        exit(EXIT_FAILURE);
    }

    GLint WIDTH, HEIGHT;
    WIDTH = 800;
    HEIGHT = 600;

    window = glfwCreateWindow(WIDTH, HEIGHT, "02 textures", NULL, NULL);
    if(!window) {
        fprintf(stderr, "Failed to create window, aborting.\n");
        exit(EXIT_FAILURE);
    }

    while(!glfwWindowShouldClose(window)) {

        glfwPollEvents();


        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
