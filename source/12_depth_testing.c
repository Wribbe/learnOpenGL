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

GLfloat decal_vertices[] = {
    -0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

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

int compare_window_distance(const void *p1, const void *p2) {
    float *vector1, *vector2;
    float quadratic_sum_1, quadratic_sum_2;
    int i;

    vector1 = (float*)p1;
    vector2 = (float*)p2;

    float dist_cam1[3], dist_cam2[3];
    for (i=0; i<3; i++) {
        dist_cam1[i] = camera_pos->data[i] - vector1[i];
        dist_cam2[i] = camera_pos->data[i] - vector2[i];
    }

    quadratic_sum_1 = 0;
    quadratic_sum_2 = 0;
    for (i=0; i<3; i++) {
        quadratic_sum_1 += dist_cam1[i]*dist_cam1[i];
        quadratic_sum_2 += dist_cam2[i]*dist_cam2[i];
    }
    if (quadratic_sum_1 < quadratic_sum_2) {
        return 1;
    } else {
        return -1;
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

int main(int argc, char* argv[]) {

    int num_vertices;
    if(load_model(&vertices, &num_vertices)) {
        fprintf(stderr, "Could not load model, aborting.\n");
        return(EXIT_FAILURE);
    }

    if(!glfwInit()) {
        fprintf(stderr, "Could not load GLFW, aborting.\n");
        return(EXIT_FAILURE);
    }
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    int WIDTH, HEIGHT;
    WIDTH = 1920;
    HEIGHT = 1080;

    GLFWwindow *window;
    if (argc > 1 && strcmp(argv[1], "--full") == 0) {
        window = glfwCreateWindow(WIDTH,HEIGHT,"12 Depth Test.", glfwGetPrimaryMonitor(), NULL);
    } else {
        window = glfwCreateWindow(WIDTH,HEIGHT,"12 Depth Test.", NULL, NULL);
    }

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

    /* GL state configuration. */

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    /* Load textures. */

    GLuint diffuse_map, specular_map, texture_grass;

    glGenTextures(1, &diffuse_map);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    load_texture("textures/container_diffuse.png");
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(0);

    glGenTextures(1, &specular_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular_map);
    load_texture("textures/container_specular.png");
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(0);

    glGenTextures(1, &texture_grass);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture_grass);
    load_texture("textures/grass.png");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(0);

    GLuint texture_window;

    glGenTextures(1, &texture_window);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture_window);
    load_texture("textures/window_red.png");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(0);

    GLuint texture_anisotropic;

    glGenTextures(1, &texture_anisotropic);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texture_anisotropic);
    load_texture("textures/anisotropic_direction.jpg");
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(0);

    GLuint texture_brushed;
    glGenTextures(1, &texture_brushed);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, texture_brushed);
    load_texture("textures/brushed.jpg");
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(0);

    /* Frambuffer setup. */

    GLuint FBO;
    glGenFramebuffers(1, &FBO);

    GLuint texture_framebuffer;
    glGenTextures(1, &texture_framebuffer);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture_framebuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, 0, GL_DEPTH_STENCIL,
                 GL_UNSIGNED_INT_24_8, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGBA,
    //             GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture_framebuffer, 0);

    /* Renderbuffer setup. */

    GLuint RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    /* Attach renderbuffer to framebuffer. */

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "ERROR: Framebuffer is not complete!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* Vertex Array and Vertex Buffer setup. */

    GLuint VAO, VBO, lightVAO, VAO_decal, VBO_decal;

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

    /* LightVAO definition. */

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Same vertex data as cube.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    /* VAO_decal and VBO_decal definitions. */

    glGenVertexArrays(1, &VAO_decal);
    glGenBuffers(1, &VBO_decal);
    glBindVertexArray(VAO_decal);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_decal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decal_vertices), decal_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(5*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    /* Shader loading and construction. */

    char* vertex_source = read_shader("shaders/12.vert");
    char* fragment_source = read_shader("shaders/12.frag");
    char* lamp_fragment_source = read_shader("shaders/12_lamp.frag");
    char* outline_program_source = read_shader("shaders/12_single_color.frag");
    char* source_framebuffer_vert = read_shader("shaders/12_framebuffer.vert");
    char* source_framebuffer_frag = read_shader("shaders/12_framebuffer.frag");

    GLuint shader_program, lamp_program, outline_program, program_framebuffer;
    shader_program = create_shader_program(vertex_source, fragment_source);
    lamp_program = create_shader_program(vertex_source, lamp_fragment_source);
    outline_program = create_shader_program(vertex_source, outline_program_source);
    program_framebuffer = create_shader_program(source_framebuffer_vert, source_framebuffer_frag);

    free(vertex_source);
    free(fragment_source);
    free(lamp_fragment_source);
    free(outline_program_source);
    free(source_framebuffer_vert);
    free(source_framebuffer_frag);

    /* Matrix allocations. */

    Mat4f *projection, *model, *view, *temp, *temp2;
    mat4f_allocate(&projection);
    mat4f_allocate(&model);
    mat4f_allocate(&view);
    mat4f_allocate(&temp);
    mat4f_allocate(&temp2);

    mat4f_translate(view, 0.0f, 0.0f, -3.0f);
    //mat4f_rotate_x(model, -M_PI/4);
    //mat4f_rotate_x(model, 0.0f);
    //mat4f_translate(model, -0.5f, -0.5f, -0.5f);
    mat4f_unity(model);

    /* Vertex allocations. */

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

    /* Shader locations. */

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
    glUniform1i(glGetUniformLocation(program_framebuffer, "screen_texture"), 0);
    glUniform1d(location_POINT_LIGHTS, POINT_LIGHTS);

    float r = 15.0f;
    float offset = -15.0f;
    float static_point_positions[POINT_LIGHTS][3] = {
        {   r,  -1.0f, offset},
        {  -r,  5.0f, offset},
        {0.0f,  -1.0f, offset-r},
        {0.0f,  5.0f, offset+r},
    };
    float point_positions[POINT_LIGHTS][3];

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

    float specular_color[POINT_LIGHTS][3] = {
        {1.0f, 0.5f, 1.0f},
        {0.3f, 1.0f, 0.2f},
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0.5f, 0.5f},
    };

    float constant, linear, quadratic;
    constant = 1.0f;
    linear = 0.09f;
    quadratic = 0.032f;

    for(i=0; i<POINT_LIGHTS; i++) {
        glUniform3fv(point_light_position[i][POSITION], 1, (GLvoid*)(&point_positions[i][0]));
        glUniform3fv(point_light_position[i][AMBIENT], 1, (GLvoid*)(&ambient_color[i][0]));
        glUniform3fv(point_light_position[i][DIFFUSE], 1, (GLvoid*)(&diffuse_color[i][0]));
        glUniform3fv(point_light_position[i][SPECULAR], 1, (GLvoid*)(&specular_color[i][0]));
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

    glUseProgram(outline_program);

    GLuint outline_model_location, outline_projection_location, outline_view_location;

    outline_model_location = glGetUniformLocation(outline_program, "model");
    outline_projection_location = glGetUniformLocation(outline_program, "perspective");
    outline_view_location = glGetUniformLocation(outline_program, "view");

    glUseProgram(0);


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

    int num_cubes = 5;

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

        /* Render to framebuffer object. */

        //glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glStencilMask(0x00);
        glBindVertexArray(lightVAO);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        /* Draw lights. */

        glUseProgram(lamp_program);
        glBindVertexArray(VAO);

        GLfloat time = glfwGetTime();
        for(i=0; i<POINT_LIGHTS; i++) {
            mat4f_scale(temp, 0.2f, 0.2f, 0.2f);
            mat4f_mul(temp, temp, model);
            float x, y, z;
            point_positions[i][0]= static_point_positions[i][0]+r*cos(((M_PI/4)*time));
            point_positions[i][1]= static_point_positions[i][1];
            point_positions[i][2]= static_point_positions[i][2]+r*sin(((M_PI/4)*time));

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

        glUseProgram(0);
        glBindVertexArray(0);

        /* Draw Decals */

        glUseProgram(shader_program);

        vec3f_add(camera_target, camera_pos, camera_front);
        mat4f_look_at(view, camera_pos, camera_target, camera_up);

        mat4f_perspective(projection, fov, (float)WIDTH/(float)HEIGHT,
                          0.1f, 100.0f);

        glUniform3f(view_position_location, camera_pos->data[0],
                                            camera_pos->data[1],
                                            camera_pos->data[2]);

        glUniform3f(location_camera_front, camera_front->data[0],
                                           camera_front->data[1],
                                           camera_front->data[2]);

        glBindVertexArray(VAO_decal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_grass);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_grass);

        glUniformMatrix4fv(view_location, 1, GL_TRUE,
                           mat4f_pointer(view));
        glUniformMatrix4fv(projection_location, 1, GL_TRUE,
                           mat4f_pointer(projection));

        float grass_displacement = 6.0f;
        float decal_scale = 1.3f;
        int num_decals = 9;
        float decal_rotation_step = M_PI/num_decals*2;
        for (int j=0; j<num_decals; j++) {
            for (i=0; i<num_cubes; i++) {
                mat4f_unity(temp);
                mat4f_mul(temp, temp, model);
                mat4f_rotate_y(temp2, decal_rotation_step*j);
                mat4f_mul(temp, temp, temp2);
                mat4f_scale(temp2, decal_scale, decal_scale, decal_scale);
                mat4f_mul(temp, temp, temp2);
                mat4f_translate(temp2, cube_locations[i][0]+grass_displacement,
                                       cube_locations[i][1],
                                       cube_locations[i][2]);
                mat4f_mul(temp, temp2, temp);
                glUniformMatrix4fv(model_location, 1, GL_TRUE,
                                   mat4f_pointer(temp));
                glDrawArrays(GL_QUADS, 0, sizeof(decal_vertices));
            }
        }

        glActiveTexture(0);
        glBindVertexArray(0);

        /* Draw coffeepots. */

        glBindVertexArray(VAO);
        glUseProgram(shader_program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_brushed);
        //glBindTexture(GL_TEXTURE_2D, texture_anisotropic);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_anisotropic);

        for (i=0; i<POINT_LIGHTS; i++) {
            glUniform3fv(point_light_position[i][POSITION], 1, (GLvoid*)(&point_positions[i][0]));
        }

        for(i=0; i<num_cubes; i++) {
            mat4f_translate(temp, cube_locations[i][0],
                                  cube_locations[i][1],
                                  cube_locations[i][2]);
            mat4f_mul(temp2, temp, model);
            glUniformMatrix4fv(view_location, 1, GL_TRUE,
                               mat4f_pointer(view));
            glUniformMatrix4fv(projection_location, 1, GL_TRUE,
                               mat4f_pointer(projection));
            glUniformMatrix4fv(model_location, 1, GL_TRUE,
                               mat4f_pointer(temp2));
            glDrawArrays(GL_QUADS, 0, num_vertices);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(0);
        glBindVertexArray(0);
        glUseProgram(0);

        /* Draw outlines. */

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(outline_program);

        glBindVertexArray(VAO);

        float outline_scale = 1.003f;
        for(i=0; i<num_cubes; i++) {

            mat4f_scale(temp, outline_scale, outline_scale, outline_scale);
            mat4f_translate(temp2,cube_locations[i][0],
                                  cube_locations[i][1],
                                  cube_locations[i][2]);
            mat4f_mul(temp2, temp2, temp);
            mat4f_mul(temp2, temp2, model);
            glUniformMatrix4fv(outline_view_location, 1, GL_TRUE,
                               mat4f_pointer(view));
            glUniformMatrix4fv(outline_projection_location, 1, GL_TRUE,
                               mat4f_pointer(projection));
            glUniformMatrix4fv(outline_model_location, 1, GL_TRUE,
                               mat4f_pointer(temp2));
            glDrawArrays(GL_QUADS, 0, num_vertices);
        }
        glBindVertexArray(0);
        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);

        /* Draw transparent objects */

        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);

        glStencilMask(0x00);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        glUseProgram(shader_program);

        glBindVertexArray(VAO_decal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_window);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_window);

        float window_locations[num_cubes][3];
        memcpy(window_locations, cube_locations, sizeof(float)*num_cubes*3);

        qsort(window_locations, num_cubes, sizeof(float)*3, compare_window_distance);

        float window_displacement = grass_displacement + 4.0f;
        for(i=0; i<num_cubes; i++) {
            mat4f_translate(temp, window_locations[i][0]+window_displacement,
                                  window_locations[i][1],
                                  window_locations[i][2]);
            mat4f_mul(temp2, temp, model);
            glUniformMatrix4fv(view_location, 1, GL_TRUE,
                               mat4f_pointer(view));
            glUniformMatrix4fv(projection_location, 1, GL_TRUE,
                               mat4f_pointer(projection));
            glUniformMatrix4fv(model_location, 1, GL_TRUE,
                               mat4f_pointer(temp2));
            glDrawArrays(GL_QUADS, 0, num_vertices);
        }
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);

        glUseProgram(0);
        glActiveTexture(0);
        glBindVertexArray(0);

        /* Render framebuffer to quad. */

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glUseProgram(program_framebuffer);
        //glBindVertexArray(VAO_decal);
        //glDisable(GL_DEPTH_TEST);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, texture_framebuffer);
        ////glBindTexture(GL_TEXTURE_2D, texture_window);
        //glDrawArrays(GL_QUADS, 0, sizeof(decal_vertices));
        //glBindVertexArray(0);

        /* Swap buffers. */

        glfwSwapBuffers(window);

    }
    glfwTerminate();

    return(EXIT_SUCCESS);
}
