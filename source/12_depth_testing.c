#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "file_utils.h"

#define UNUSED(x) (void)x
#define M_PI 3.14159265358979323846264338327
#define POINT_LIGHTS 4

Vec3f *camera_pos, *camera_target, *camera_up, *camera_front, *temp_vec3f;
GLfloat delta_time, last_frame, current_frame, last_x, last_y, yaw, pitch, fov;
bool keys[1024], first_mouse;
int i;

GLsizei stride = 8 * sizeof(GLfloat);
GLfloat *vertices;

void vec3f_add(Vec3f *result, Vec3f *vecA, Vec3f *vecB) {
    int i;
    float res;
    for (i=0; i<3; i++) {
        res = (vecA->data[i]) + (vecB->data[i]);
        result->data[i] = res;
    }
}

void vec3f_copy(Vec3f *destination, Vec3f *source) {
    int i;
    for (i=0; i<3; i++) {
        destination->data[i] = source->data[i];
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
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (action == GLFW_PRESS) {
        keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    GLfloat x_offset, y_offset, sensitivity, x, y, z;

    UNUSED(window);

    xpos = (float)xpos;
    ypos = (float)ypos;

    if(first_mouse) {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    x_offset = xpos - last_x;
    y_offset = last_y - ypos;
    last_x = xpos;
    last_y = ypos;

    sensitivity = 0.001f;
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    if(pitch > M_PI/2) {
        pitch = M_PI/2;
    }
    if(pitch < -M_PI/2) {
        pitch = -M_PI/2;
    }

    x = cosf(yaw) * cosf(pitch);
    y = sinf(pitch);
    z = sinf(yaw) * cosf(pitch);
    vec3f_set(camera_front, x, y, z);
    vec3f_normalize(camera_front, camera_front);
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {

    UNUSED(window);
    UNUSED(x_offset);

    float fov_min, fov_max;
    fov_min = (float)M_PI*(1.0f/180.0f);
    fov_max = (float)M_PI/4.0f;

    if(fov >= fov_min && fov <= fov_max) {
        fov -= fov_min*(float)y_offset;
    }
    if(fov <= fov_min) {
        fov = fov_min;
    }
    if(fov >= fov_max) {
        fov = fov_max;
    }
    printf("fov: %f\n",fov);
    printf("y_offset: %f\n",y_offset);
    printf("fov_min: %f\n",fov_min);
}

void do_movement() {
    GLfloat camera_speed;
    camera_speed = 5.0f * delta_time;
    if(keys[GLFW_KEY_W]) {
        vec3f_muls(temp_vec3f, camera_front, camera_speed);
        vec3f_add(camera_pos, camera_pos, temp_vec3f);
    }
    if(keys[GLFW_KEY_S]) {
        vec3f_muls(temp_vec3f, camera_front, camera_speed);
        vec3f_sub(camera_pos, camera_pos, temp_vec3f);
    }
    if(keys[GLFW_KEY_A]) {
        vec3f_cross(temp_vec3f, camera_front, camera_up);
        vec3f_normalize(temp_vec3f, temp_vec3f);
        vec3f_muls(temp_vec3f, temp_vec3f, camera_speed);
        vec3f_sub(camera_pos, camera_pos, temp_vec3f);
    }
    if(keys[GLFW_KEY_D]) {
        vec3f_cross(temp_vec3f, camera_front, camera_up);
        vec3f_normalize(temp_vec3f, temp_vec3f);
        vec3f_muls(temp_vec3f, temp_vec3f, camera_speed);
        vec3f_add(camera_pos, camera_pos, temp_vec3f);
    }
}

int main(void) {

    int num_vertices;
    if(load_model(&vertices, &num_vertices)) {
        fprintf(stderr, "Could not load model, aborting.\n");
        return(EXIT_FAILURE);
    }

    if(!glfwInit()) {
        fprintf(stderr, "Could not load GLFW, aborting.\n");
        return(EXIT_FAILURE);
    }

    int WIDTH, HEIGHT;
    WIDTH = 800;
    HEIGHT = 600;

    GLFWwindow *window;
    window = glfwCreateWindow(WIDTH,HEIGHT,"12 Depth Test.", NULL, NULL);

    if(!window) {
        fprintf(stderr, "Could not create main window, aborting.\n");
        return(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Could not initialize GLEW, aborting.\n");
        return(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLuint VAO, VBO, lightVAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, num_vertices*sizeof(GLfloat)*8, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    GLuint diffuse_map, specular_map;
    glGenTextures(1, &diffuse_map);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    load_texture("textures/container_diffuse.png");
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &specular_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular_map);
    load_texture("textures/container_specular.png");
    glBindTexture(GL_TEXTURE_2D, 0);

    /* LightVAO definition. */

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Same vertex data as cube.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    char* vertex_source = read_shader("shaders/12.vert");
    char* fragment_source = read_shader("shaders/12.frag");
    char* lamp_fragment_source = read_shader("shaders/12_lamp.frag");

    GLuint shader_program, lamp_program;
    shader_program = create_shader_program(vertex_source, fragment_source);
    lamp_program = create_shader_program(vertex_source, lamp_fragment_source);

    free(vertex_source);
    free(fragment_source);
    free(lamp_fragment_source);

    Mat4f *projection, *model, *view, *temp, *temp2;
    mat4f_allocate(&projection);
    mat4f_allocate(&model);
    mat4f_allocate(&view);
    mat4f_allocate(&temp);
    mat4f_allocate(&temp2);

    mat4f_translate(view, 0.0f, 0.0f, -3.0f);
    //mat4f_rotate_x(model, -M_PI/4);
    mat4f_rotate_x(model, 0.0f);

    Vec3f *light_position;
    vec3f_allocate(&light_position);

    vec3f_allocate(&camera_pos);
    vec3f_allocate(&camera_target);
    vec3f_allocate(&camera_up);
    vec3f_allocate(&camera_front);
    vec3f_allocate(&temp_vec3f);

    vec3f_set(camera_target, 0.0f, 0.0f, 0.0f);
    vec3f_set(camera_up, 0.0f, 1.0f, 0.0f);
    vec3f_set(camera_front, 0.0f, 0.0f, -1.0f);
    vec3f_set(camera_pos, 0.0f, 0.0f, 3.0f);
    vec3f_set(light_position, 1.2f, 1.0f, 2.0f);

    /* shader locations */

    GLuint model_location = glGetUniformLocation(shader_program, "model");
    GLuint projection_location = glGetUniformLocation(shader_program, "perspective");
    GLuint view_location = glGetUniformLocation(shader_program, "view");

    GLuint light_position_location = glGetUniformLocation(shader_program, "light_position");
    GLuint view_position_location = glGetUniformLocation(shader_program, "view_position");

    GLuint material_shininess_location = glGetUniformLocation(shader_program, "material.shininess");


    GLuint sampler_diffuse_location = glGetUniformLocation(shader_program, "material.diffuse");
    GLuint sampler_specular_location = glGetUniformLocation(shader_program, "material.specular");

    GLuint location_camera_front = glGetUniformLocation(shader_program, "camera_front");
    //GLuint location_cutoff_angle = glGetUniformLocation(shader_program, "light.cutoff_angle");
    //GLuint location_outer_cutoff_angle = glGetUniformLocation(shader_program, "light.outer_cutoff_angle");

    GLuint location_POINT_LIGHTS = glGetUniformLocation(shader_program, "POINT_LIGHTS");

    enum {POSITION, AMBIENT, DIFFUSE, SPECULAR, CONSTANT, LINEAR, QUADRATIC, NUM_TYPES};
    const char *types[NUM_TYPES] = {"position","ambient","diffuse","specular","constant","linear","quadratic"};

    char uniform_string[256];
    GLuint point_light_position[POINT_LIGHTS][NUM_TYPES];
    for(i=0; i<POINT_LIGHTS; i++) {
        for(int j=0; j<NUM_TYPES; j++) {
            snprintf(&uniform_string[0], sizeof(uniform_string), "point_lights[%d].%s",i,types[j]);
            point_light_position[i][j] = glGetUniformLocation(shader_program, uniform_string);
        }
    }
    /* set non updating uniform values */

    glUseProgram(shader_program);

    //glUniform3f(light_position_location, light_position->data[0],
    //                                     light_position->data[1],
    //                                     light_position->data[2]);

    glUniform3fv(light_position_location, 3, (GLvoid*)(&light_position->data[0]));

    glUniform1f(material_shininess_location, 32.0f);
    glUniform1i(sampler_diffuse_location, 0);
    glUniform1i(sampler_specular_location, 1);
    glUniform1d(location_POINT_LIGHTS, POINT_LIGHTS);

    float point_positions[POINT_LIGHTS][3] = {
        { 5.0f,  0.0f, -10.0f},
        { 0.0f, -5.0f, -10.0f},
        {-4.0f,  5.0f, -10.0f},
        {-5.0f,  0.0f, -10.0f},
    };

    float ambient_color[POINT_LIGHTS][3] = {
        {1.0f, 0.5f, 1.0f},
        {0.3f, 1.0f, 0.2f},
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0.5f, 0.5f},
    };

    float diffuse_color[POINT_LIGHTS][3] = {
        {1.0f, 0.5f, 1.0f},
        {0.3f, 1.0f, 0.2f},
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0.5f, 0.5f},
    };

    float specular_color[3] = {1.0,1.0,1.0};

    float constant, linear, quadratic;
    constant = 1.0f;
    linear = 0.09f;
    quadratic = 0.032f;

    for(i=0; i<POINT_LIGHTS; i++) {
        glUniform3fv(point_light_position[i][POSITION], 1, (GLvoid*)(&point_positions[i][0]));
        glUniform3fv(point_light_position[i][AMBIENT], 1, (GLvoid*)(&ambient_color[i][0]));
        glUniform3fv(point_light_position[i][DIFFUSE], 1, (GLvoid*)(&diffuse_color[i][0]));
        glUniform3fv(point_light_position[i][SPECULAR], 1, (GLvoid*)(&specular_color[0]));
        glUniform1f(point_light_position[i][CONSTANT], constant);
        glUniform1f(point_light_position[i][LINEAR], linear);
        glUniform1f(point_light_position[i][QUADRATIC], quadratic);
    }

    glUseProgram(0);

    glUseProgram(lamp_program);

    GLuint lamp_model_location, lamp_projection_location, lamp_view_location;

    lamp_model_location = glGetUniformLocation(lamp_program, "model");
    lamp_projection_location = glGetUniformLocation(lamp_program, "perspective");
    lamp_view_location = glGetUniformLocation(lamp_program, "view");

    glUseProgram(0);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    current_frame = 0.0f;
    last_frame = 0.0f;
    last_x = WIDTH / 2.0f;
    last_y = HEIGHT / 2.0f;
    fov = M_PI/4;
    yaw = -M_PI/2;
    pitch = 0.0f;
    first_mouse = true;

    Vec3f *light_color;
    vec3f_allocate(&light_color);

    srand(time(NULL));

    int num_cubes = 4;

    float cube_locations[num_cubes][3];
    //float cube_rotations[num_cubes][3];
    //float x_max, y_max, z_max;

    //x_max = 4.0f;
    //y_max = 4.0f;
    //z_max = 25.0f;

    for (i=0; i<num_cubes; i++) {

//        cube_locations[i][0] = ((x_max*2.0) * ((float)rand()/(float)RAND_MAX)) - x_max;
//        cube_locations[i][1] = ((y_max*2.0) * ((float)rand()/(float)RAND_MAX)) - y_max;
//        cube_locations[i][2] = -(z_max * ((float)rand()/(float)RAND_MAX));
        cube_locations[i][0] = 0.0f;
        cube_locations[i][1] = 0.0f;
        cube_locations[i][2] = -i*5.0f-3.0f;

     //   cube_rotations[i][0] = ((float)M_PI * ((float)rand()/(float)RAND_MAX));
     //   cube_rotations[i][1] = ((float)M_PI * ((float)rand()/(float)RAND_MAX));
     //   cube_rotations[i][2] = ((float)M_PI * ((float)rand()/(float)RAND_MAX));
    }

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        do_movement();


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);

        vec3f_add(camera_target, camera_pos, camera_front);
        mat4f_look_at(view, camera_pos, camera_target, camera_up);

        mat4f_perspective(projection, fov, (float)WIDTH/(float)HEIGHT,
                          0.1f, 100.0f);

        glUniformMatrix4fv(model_location, 1, GL_TRUE,
                           mat4f_pointer(model));
        glUniformMatrix4fv(view_location, 1, GL_TRUE,
                           mat4f_pointer(view));
        glUniformMatrix4fv(projection_location, 1, GL_TRUE,
                           mat4f_pointer(projection));

        glUniform3f(view_position_location, camera_pos->data[0],
                                            camera_pos->data[1],
                                            camera_pos->data[2]);

        glUniform3f(location_camera_front, camera_front->data[0],
                                           camera_front->data[1],
                                           camera_front->data[2]);

        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);

        for(i=0; i<num_cubes; i++) {
            mat4f_translate(temp, cube_locations[i][0],
                                  cube_locations[i][1],
                                  cube_locations[i][2]);
            mat4f_mul(temp2, temp, model);
            //mat4f_rotate_x(temp, cube_rotations[i][0]);
            //mat4f_mul(temp2, temp2, temp);
            //mat4f_rotate_y(temp, cube_rotations[i][1]);
            //mat4f_mul(temp2, temp2, temp);
            //mat4f_rotate_z(temp, cube_rotations[i][2]);
            //mat4f_mul(temp2, temp2, temp);
            glUniformMatrix4fv(model_location, 1, GL_TRUE,
                               mat4f_pointer(temp2));
            //glDrawArrays(GL_TRIANGLES, 0, 36);
            //glDrawArrays(GL_QUADS, 0, 24);
            glDrawArrays(GL_QUADS, 0, num_vertices);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(0);

        glBindVertexArray(0);

        glUseProgram(lamp_program);

        glBindVertexArray(lightVAO);

        for(i=0; i<POINT_LIGHTS; i++) {
            mat4f_scale(temp, 0.2f, 0.2f, 0.2f);
            mat4f_mul(temp, temp, model);
            float x, y, z;
            x = point_positions[i][0];
            y = point_positions[i][1];
            z = point_positions[i][2];
            GLuint location_lamp_color = glGetUniformLocation(lamp_program, "lamp_color");
            glUniform3fv(location_lamp_color, 1, (GLvoid*)(&diffuse_color[i][0]));
            mat4f_translate(temp2, x, y, z);
            mat4f_mul(temp2, temp2, temp);

            glUniformMatrix4fv(lamp_model_location, 1, GL_TRUE,
                               mat4f_pointer(temp2));
            glUniformMatrix4fv(lamp_view_location, 1, GL_TRUE,
                               mat4f_pointer(view));
            glUniformMatrix4fv(lamp_projection_location, 1, GL_TRUE,
                               mat4f_pointer(projection));

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            glDrawArrays(GL_QUADS, 0, num_vertices);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);

    }
    glfwTerminate();

    return(EXIT_SUCCESS);
}
