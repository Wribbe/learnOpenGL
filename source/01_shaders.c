#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
    -0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
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

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader, 1, (const GLchar**) &vert_source, NULL);
    glShaderSource(fragment_shader, 1, (const GLchar**) &frag_source, NULL);

    int info_log_size = 512;
    GLint success;
    GLchar info_log[info_log_size];

    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertex_shader, info_log_size, NULL, info_log);
        fprintf(stderr, "Error: Vertex shader compilation failed with:\n %s",info_log);
    }

    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(fragment_shader, info_log_size, NULL, info_log);
        fprintf(stderr, "Error: Fragment shader compilation failed with:\n %s",info_log);
    }

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
