#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "file_utils.h"

#define UNUSED(x) (void)x
#define M_PI 3.14159265358979323846264338327

GLsizei stride = 5*sizeof(GLfloat);

GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

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

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, sizeof(GLfloat), GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, sizeof(GLfloat), GL_FLOAT, GL_FALSE, stride,
                          (GLvoid*)(3*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    char* vertex_source = read_shader("shaders/05.vert");
    char* fragment_source = read_shader("shaders/05.frag");

    GLuint shader_program;
    shader_program = create_shader_program(vertex_source, fragment_source);

    glUseProgram(shader_program);

    GLuint texture;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    load_texture("textures/02_container.jpg");
    glUniform1i(glGetUniformLocation(shader_program, "texture_sampler"), 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    Mat4f projection, model, view;
    mat4f_allocate(&projection);
    mat4f_allocate(&model);
    mat4f_allocate(&view);

    mat4f_translate(&view, 0.0f, 0.0f, -3.0f);
    mat4f_perspective(&projection, (float)M_PI/4, (float)WIDTH/(float)HEIGHT,
                      0.1f, 100.0f);
    mat4f_rotate_x(&model, -M_PI/4);

    GLuint model_location, projection_location, view_location;
    model_location = glGetUniformLocation(shader_program, "model");
    projection_location = glGetUniformLocation(shader_program, "perspective");
    view_location = glGetUniformLocation(shader_program, "view");

    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUniformMatrix4fv(model_location, 1, GL_TRUE,
                           mat4f_pointer(&model));
        glUniformMatrix4fv(view_location, 1, GL_TRUE,
                           mat4f_pointer(&view));
        glUniformMatrix4fv(projection_location, 1, GL_TRUE,
                           mat4f_pointer(&projection));

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glfwTerminate();

    return(EXIT_SUCCESS);
}
