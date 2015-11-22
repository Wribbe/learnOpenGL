#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "file_utils.h"

#define UNUSED(x) (void)x
#define M_PI 3.14159265358979323846264338327

Vec3f *camera_pos, *camera_target, *camera_up, *camera_front, *temp_vec3f;

GLsizei stride = 5 * sizeof(GLfloat);
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

void vec3f_add(Vec3f *result, Vec3f *vecA, Vec3f *vecB) {
    int i;
    float res;
    for (i=0; i<3; i++) {
        res = (vecA->data[i]) + (vecB->data[i]);
        result->data[i] = res;
    }
}

void vec3f_adds(Vec3f *result, Vec3f *vector, float scalar) {
    int i;
    for (i=0; i<3; i++) {
        result->data[i] = vector->data[i] + scalar;
    }
}

void vec3f_subs(Vec3f *result, Vec3f *vector, float scalar) {
    int i;
    for (i=0; i<3; i++) {
        result->data[i] = vector->data[i] - scalar;
    }
}

void vec3f_muls(Vec3f *result, Vec3f *vector, float scalar) {
    int i;
    for (i=0; i<3; i++) {
        result->data[i] = vector->data[i] * scalar;
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    UNUSED(scancode);
    UNUSED(mods);
    GLfloat camera_speed = 0.05f;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(key == GLFW_KEY_W) {
        vec3f_muls(temp_vec3f, camera_front, camera_speed);
        vec3f_add(camera_pos, camera_pos, temp_vec3f);
    }
    if(key == GLFW_KEY_S) {
        vec3f_muls(temp_vec3f, camera_front, camera_speed);
        vec3f_sub(camera_pos, camera_pos, temp_vec3f);
    }
    if(key == GLFW_KEY_A) {
        vec3f_cross(temp_vec3f, camera_front, camera_up);
        vec3f_normalize(temp_vec3f, temp_vec3f);
        vec3f_muls(temp_vec3f, temp_vec3f, camera_speed);
        vec3f_sub(camera_pos, camera_pos, temp_vec3f);
    }
    if(key == GLFW_KEY_D) {
        vec3f_cross(temp_vec3f, camera_front, camera_up);
        vec3f_normalize(temp_vec3f, temp_vec3f);
        vec3f_muls(temp_vec3f, temp_vec3f, camera_speed);
        vec3f_add(camera_pos, camera_pos, temp_vec3f);
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

    glEnable(GL_DEPTH_TEST);

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (GLvoid*)(3*sizeof(GLfloat)));
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

    Mat4f *projection, *model, *view;
    mat4f_allocate(&projection);
    mat4f_allocate(&model);
    mat4f_allocate(&view);

    mat4f_translate(view, 0.0f, 0.0f, -3.0f);
    mat4f_perspective(projection, (float)M_PI/4, (float)WIDTH/(float)HEIGHT,
                      0.1f, 100.0f);
    //mat4f_rotate_x(model, -M_PI/4);
    mat4f_rotate_x(model, 0.0f);

    GLuint model_location, projection_location, view_location;
    model_location = glGetUniformLocation(shader_program, "model");
    projection_location = glGetUniformLocation(shader_program, "perspective");
    view_location = glGetUniformLocation(shader_program, "view");


    vec3f_allocate(&camera_pos);
    vec3f_allocate(&camera_target);
    vec3f_allocate(&camera_up);
    vec3f_allocate(&camera_front);
    vec3f_allocate(&temp_vec3f);

    vec3f_set(camera_target, 0.0f, 0.0f, 0.0f);
    vec3f_set(camera_up, 0.0f, 1.0f, 0.0f);
    vec3f_set(camera_front, 0.0f, 0.0f, -1.0f);
    vec3f_set(camera_pos, 0.0f, 0.0f, 3.0f);

    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

//        cam_x = sinf(time) * radius;
//        cam_z = cosf(time) * radius;
        vec3f_add(camera_target, camera_pos, camera_front);
        mat4f_look_at(view, camera_pos, camera_target, camera_up);

        //mat4f_rotate_x(model, sinf(time) * M_PI);
        glUniformMatrix4fv(model_location, 1, GL_TRUE,
                           mat4f_pointer(model));
        glUniformMatrix4fv(view_location, 1, GL_TRUE,
                           mat4f_pointer(view));
        glUniformMatrix4fv(projection_location, 1, GL_TRUE,
                           mat4f_pointer(projection));

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glfwTerminate();

    return(EXIT_SUCCESS);
}
