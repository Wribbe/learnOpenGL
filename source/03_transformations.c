#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "file_utils.h"

#define UNUSED(x) (void)x

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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    UNUSED(scancode);
    UNUSED(mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

typedef struct Matrix4f {
    float data[4][4];
} Mat4f;

void zero_Mat4f(Mat4f *matrix) {
    int i, j;
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            matrix->data[i][j] = 0;
        }
    }
}

void create_Mat4f(Mat4f *matrix) {
    matrix = malloc(sizeof(Mat4f));
    if(!matrix) {
        fprintf(stderr, "Could not allocate memory for Mat4f matrix.\n");
        return NULL;
    }
    zero_Mat4f(matrix);
}

void scale_Mat4f(Mat4f *matrix, float x, float y, float z) {
    int i, j;
    for(i=0; i<4; i++) {
        for(j=0; j<4; j++) {
            if(i == j) {
                switch(i) {
                    case 0:
                        matrix->data[i][j] = x;
                        break;
                    case 1:
                        matrix->data[i][j] = y;
                        break;
                    case 2:
                        matrix->data[i][j] = z;
                        break;
                    case 3:
                        matrix->data[i][j] = 1.0f;
                        break;
                }
            } else {
                matrix->data[i][j] = 0;
            }
        }
    }
}

GLvoid *pointer_Mat4f(Mat4f *matrix) {
    return (GLvoid*)(matrix->data[0]);
}

int main(void) {

    GLFWwindow *window;

    if(!glfwInit()) {
        fprintf(stderr, "Could not initialize GLFW, aborting.\n");
        return(EXIT_FAILURE);
    }

    GLint WIDTH, HEIGHT;
    WIDTH = 800;
    HEIGHT = 600;

    window = glfwCreateWindow(WIDTH, HEIGHT, "03 Transformation.", NULL, NULL);

    if(!window) {
        fprintf(stderr, "Could not create window, aborting.\n");
        return(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Could not initialize GLEW, aborting.\n");
        return(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

    char* vertex_source = read_shader("shaders/03.vert");
    char* fragment_source = read_shader("shaders/03.frag");

    GLuint shader_program = create_shader_program(vertex_source, fragment_source);
    UNUSED(shader_program);

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GLsizei stride = 8 * sizeof(GLfloat);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (GLvoid*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glUseProgram(shader_program);

    GLuint texture00;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture00);
    glBindTexture(GL_TEXTURE_2D, texture00);

    load_texture("textures/02_container.jpg");
    glUniform1i(glGetUniformLocation(shader_program, "our_texture_01"), 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    Mat4f scale;
    create_Mat4f(&scale);
    scale_Mat4f(&scale, 1.0f, 2.0f, 1.0f);

    GLuint transform_location = glGetUniformLocation(shader_program, "transform");
    glUniformMatrix4fv(transform_location, 1, GL_FALSE, pointer_Mat4f(&scale));

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture00);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return(EXIT_SUCCESS);
}
