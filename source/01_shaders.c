#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "file_utils.h"

#define UNUSED(x) (void)x

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    UNUSED(scancode);
    UNUSED(mods);
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

GLfloat vertices[] = {
    // Positions         // Colors
    -0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f,
     0.0f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  0.0f,  0.0f, 1.0f,
};

char* read_shader(char* filename) {
    char *contents = read_file(filename);
    if(!contents) {
        fprintf(stderr, "Could not load file: %s, aborting\n", filename);
        exit(0);
    }
    return contents;
}

int main(void) {

    GLuint WIDTH, HEIGHT;

    WIDTH = 800;
    HEIGHT = 600;

    GLFWwindow* window;

    if(!glfwInit()) {
        fprintf(stderr, "Error: Could not initialize GLFW, aborting.\n");
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
    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Error: Could not initialize GLEW, aborting.\n");
        return(EXIT_FAILURE);
    }

    char* vert_source = read_shader("shaders/01.vert");
    char* frag_source = read_shader("shaders/01.frag");

    GLuint vertex_shader, fragment_shader, shader_program;

    vertex_shader = compile_shader(vert_source, GL_VERTEX_SHADER);
    fragment_shader = compile_shader(frag_source, GL_FRAGMENT_SHADER);

    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, info_log_size, NULL, info_log);
        fprintf(stderr, "Error: Shander linkage failde with:\n %s",info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    free(vert_source);
    free(frag_source);

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

//    GLfloat time_value, green_value;
//    GLint vertex_color_location;

//    vertex_color_location = glGetUniformLocation(shader_program, "our_color");

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Set some uniforms. */
//        time_value = glfwGetTime();
//        green_value = (sin(time_value) / 2) + 0.5;

        glUseProgram(shader_program);
//        glUniform4f(vertex_color_location, 0.0f, green_value, 0.0f, 1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
