#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "file_utils.h"
#include <math.h>

#define UNUSED(x) (void)x
#define M_PI 3.14159265358979323846264338327

GLsizei stride = 8 * sizeof(GLfloat);

GLfloat vertices[] = {
    // Position         // Colors           // Texture Coords
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left
};

GLuint indices[] = {
    0, 1, 3,
    1, 2, 3,
};

void mat4f_perspective(float fov, float aspect, float near, float far, Mat4f* result) {
    float frustum_depth, one_over_depth;

    frustum_depth = far - near;
    one_over_depth = 1 / frustum_depth;

    mat4f_zero(result);

    result->data[1][1] = 1 / tan(0.5f * fov);
    result->data[0][0] = -1 * result->data[1][1] / aspect;
    result->data[2][2] = far * one_over_depth;
    result->data[3][2] = (-far * near) * one_over_depth;
    result->data[2][3] = 1;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    UNUSED(scancode);
    UNUSED(mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(void) {

    if (!glfwInit()) {
        fprintf(stderr, "Could not initialize GLFW, aborting.\n");
        return(EXIT_FAILURE);
    }

    GLFWwindow* window;

    int WIDTH, HEIGHT;
    WIDTH = 800;
    HEIGHT = 600;

    window = glfwCreateWindow(WIDTH, HEIGHT, "04 coordinate systems.", NULL, NULL);
    glfwSetKeyCallback(window, key_callback);
    if (!window) {
        fprintf(stderr, "Could not create window, aborting.\n");
        return(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Could not initialize GLEW, aborting.\n");
        return(EXIT_FAILURE);
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1 ,&VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (GLvoid*)(6*sizeof(GLfloat)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    char *vertex_source = read_shader("shaders/04.vert");
    char *fragment_source = read_shader("shaders/04.frag");

    GLuint shader_program;
    shader_program = create_shader_program(vertex_source, fragment_source);

    glUseProgram(shader_program);

    GLuint texture;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    load_texture("textures/02_container.jpg");
    glUniform1i(glGetUniformLocation(shader_program, "texture"), 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    Mat4f model;
    mat4f_allocate(&model);

    mat4f_rotate_x(&model, -M_PI/4);
    GLuint transform_location = glGetUniformLocation(shader_program, "transform");


    glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        glUniformMatrix4fv(transform_location, 1, GL_FALSE, mat4f_pointer(&model));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
