#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define UNUSED(x) (void)x

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    UNUSED(scancode);
    UNUSED(mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(void) {

    if(!glfwInit()) {
        fprintf(stderr, "Could not load GLFW, aborting.\n");
        return(EXIT_FAILURE);
    }

    int WIDTH, HEIGHT;
    WIDTH = 800;
    HEIGHT = 600;

    GLFWwindow *window;
    window = glfwCreateWindow(WIDTH,HEIGHT,"05 camera.", NULL, NULL);

    if(!window) {
        fprintf(stderr, "Could not create main window, aborting.\n");
        return(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Could not initialize GLEW, aborting.\n");
        return(EXIT_FAILURE);
    }

    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    return(EXIT_SUCCESS);
}
