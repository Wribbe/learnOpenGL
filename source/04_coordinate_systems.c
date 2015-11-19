#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "file_utils.h"
#include <math.h>
#include <time.h>

#define UNUSED(x) (void)x
#define M_PI 3.14159265358979323846264338327

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

GLuint indices[] = {
    0, 1, 3,
    1, 2, 3,
};

void mat4f_unity(Mat4f *matrix) {
    int i, j;
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            if (i==j) {
                matrix->data[i][j] = 1.0f;
            } else {
                matrix->data[i][j] = 0.0f;
            }
        }
    }
}

void mat4f_translate(Mat4f *matrix, float x, float y, float z) {
    mat4f_unity(matrix);
    matrix->data[0][3] = x;
    matrix->data[1][3] = y;
    matrix->data[2][3] = z;
}

void mat4f_perspective(Mat4f* result, float fov, float aspect, float near, float far) {
    float tan_half_fov;

    mat4f_zero(result);

    tan_half_fov = tan(fov/2); // t/n

    result->data[0][0] = 1 / (aspect * tan_half_fov); // n/r
    result->data[1][1] = 1 / tan_half_fov; // n/t
    result->data[2][2] = -(far + near)/(far-near);
    result->data[2][3] = -2*(far * near)/(far - near);

    result->data[3][2] = -1.0f;

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    int WIDTH, HEIGHT;
    WIDTH = 800;
    HEIGHT = 600;

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    GLFWwindow* window;

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

    glEnable(GL_DEPTH_TEST);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1 ,&VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

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

    float rotation_step;
    rotation_step = M_PI/6;

    Mat4f model, view, projection, rotation, temp;
    mat4f_allocate(&model);
    mat4f_allocate(&view);
    mat4f_allocate(&projection);
    mat4f_allocate(&rotation);
    mat4f_allocate(&temp);

    mat4f_translate(&view, 0.0f, 0.0f, -3.0f);
    mat4f_perspective(&projection, (float)M_PI/4, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

    GLuint model_location, view_location, projection_location;
    model_location = glGetUniformLocation(shader_program, "model");
    view_location = glGetUniformLocation(shader_program, "view");
    projection_location = glGetUniformLocation(shader_program, "projection");

    int num_cubes = 10;
    srand (time(NULL));
    Vec4f positions[num_cubes];
    float a = 4.0f;
    for (int i = 0; i < num_cubes; i++) {
        Vec4f vector;
        for (int j = 0; j < 4; j++) {
            if (j == 2) {
                vector.data[j] = ((float)rand()/(float)(RAND_MAX/1.0f)-2.0f);
            } else {
                vector.data[j] = ((float)rand()/(float)(RAND_MAX/a))-2.0f;
                //vector.data[j] = 1.0f*i*j;
            }
            printf("%f, ",vector.data[j]);
        }
        printf("\n");
        positions[i] = vector;
    };

    glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        float time = glfwGetTime();
        for(int i = 0; i < num_cubes; i++) {
            float *pos = positions[i].data;

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            mat4f_rotate_x(&rotation, rotation_step*time*(i+1));
            mat4f_rotate_y(&temp, rotation_step*time*(i+1));
            mat4f_mul(&rotation, &rotation, &temp);

            mat4f_translate(&temp, pos[0], pos[1], pos[2]);
            mat4f_rotate_x(&model, -M_PI/4);
            mat4f_mul(&model, &model, &temp);
            mat4f_mul(&model, &model, &rotation);

            glUniformMatrix4fv(model_location, 1, GL_TRUE, mat4f_pointer(&model));
            glUniformMatrix4fv(view_location, 1, GL_TRUE, mat4f_pointer(&view));
            glUniformMatrix4fv(projection_location, 1, GL_TRUE, mat4f_pointer(&projection));

            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
