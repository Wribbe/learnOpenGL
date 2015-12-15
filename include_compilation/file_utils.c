#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <SOIL.h>

#include "file_utils.h"

char *read_file(const char* filename) {
    FILE *file_pointer = fopen(filename, "rb");
    if (!file_pointer) {
        return NULL;
    } else {
        fseek(file_pointer, 0, SEEK_END);
        long file_size = ftell(file_pointer);
        rewind(file_pointer);

        char *file_content = malloc(file_size + 1);
        fread(file_content, file_size, 1, file_pointer);
        fclose(file_pointer);

        file_content[file_size] = '\0';
        return file_content;
    }
}

GLuint compile_shader(char* vertex_source, GLenum shader_type) {
   GLuint shader = glCreateShader(shader_type);
   glShaderSource(shader, 1, (const GLchar**) &vertex_source, NULL);
   glCompileShader(shader);
   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

   const char* shader_string;
   if(!success) {
       glGetShaderInfoLog(shader, info_log_size, NULL, info_log);
       if (shader_type == GL_VERTEX_SHADER) {
           shader_string = "Vertex";
       } else if (shader_type == GL_FRAGMENT_SHADER) {
           shader_string = "Fragment";
       } else {
           shader_string = "Unknown";
       }
       fprintf(stderr, "Error: %s shader compilation failed with:\n %s",
               shader_string,info_log);
   }
   return shader;
}

char* read_shader(const char *filename) {
    char *contents = read_file(filename);
    if(!contents) {
        fprintf(stderr, "Could not load file: %s, aborting\n", filename);
        exit(0);
    }
    return contents;
}

GLuint create_shader_program(char *vertex_source, char *fragment_source) {
    GLuint vertex_shader, fragment_shader, shader_program;

    vertex_shader = compile_shader(vertex_source, GL_VERTEX_SHADER);
    fragment_shader = compile_shader(fragment_source, GL_FRAGMENT_SHADER);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, info_log_size, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, info_log_size, NULL, info_log);
        fprintf(stderr, "Error: Shader linkage failed with:\n %s",info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

void load_texture(const char *filename) {
    int width, height;
    unsigned char* image = SOIL_load_image(filename,
                                           &width,
                                           &height,
                                           0,
                                           SOIL_LOAD_RGBA);
    if (!image) {
        fprintf(stderr, "Could not load image: %s.\n",filename);
        fprintf(stderr, "result: %s\n",SOIL_last_result());
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);
}

void mat4f_zero(Mat4f *matrix) {
    int i, j;
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            matrix->data[i][j] = 0;
        }
    }
}

void mat4f_allocate(Mat4f **matrix) {
    *matrix = malloc(sizeof(Mat4f));
    if(!matrix) {
        fprintf(stderr, "Could not allocate memory for Mat4f matrix.\n");
        return;
    }
    mat4f_zero(*matrix);
}

void mat4f_scale(Mat4f *matrix, float x, float y, float z) {
    mat4f_zero(matrix);
    matrix->data[0][0] = x;
    matrix->data[1][1] = y;
    matrix->data[2][2] = z;
    matrix->data[3][3] = 1.0f;
}

void mat4f_zero_col(Mat4f *matrix, int col) {
    int i, j;
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            if (j == col) {
                matrix->data[i][j] = 0;
            }
        }
    }
}

void mat4f_zero_row(Mat4f *matrix, int row) {
    int i, j;
    for (i=0; i<4; i++) {
        if (i == row) {
            for (j=0; j<4; j++) {
                matrix->data[i][j] = 0;
            }
            break;
        }
    }
}

void mat4f_basic_rotate(Mat4f *matrix, float radians) {
    float cos, sin;

    mat4f_zero(matrix);

    cos = cosf(radians);
    sin = sinf(radians);

    matrix->data[0][0] = cos;
    matrix->data[1][1] = cos;
    matrix->data[2][2] = cos;

    matrix->data[0][1] = (sin == 0) ? sin : -sin;
    matrix->data[1][2] = (sin == 0) ? sin : -sin;
    matrix->data[2][0] = (sin == 0) ? sin : -sin;

    matrix->data[0][2] = sin;
    matrix->data[1][0] = sin;
    matrix->data[2][1] = sin;

    matrix->data[3][3] = 1.0f;
}

void mat4f_rotate_x(Mat4f *matrix, float radians) {
    mat4f_basic_rotate(matrix, radians);
    mat4f_zero_col(matrix, 0);
    mat4f_zero_row(matrix, 0);
    matrix->data[0][0] = 1.0f;
}

void mat4f_rotate_y(Mat4f *matrix, float radians) {
    mat4f_basic_rotate(matrix, radians);
    mat4f_zero_col(matrix, 1);
    mat4f_zero_row(matrix, 1);
    matrix->data[1][1] = 1.0f;
}

void mat4f_rotate_z(Mat4f *matrix, float radians) {
    mat4f_basic_rotate(matrix, radians);
    mat4f_zero_col(matrix, 2);
    mat4f_zero_row(matrix, 2);
    matrix->data[2][2] = 1.0f;
}

void mat4f_add(Mat4f *matA, Mat4f *matB, Mat4f *matRes) {
    int i, j;
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            matRes->data[i][j] = matA->data[i][j] + matB->data[i][j];
        }
    }
}

void vec4f_add(Vec4f *vecA, Vec4f *vecB,  Vec4f *vecRes) {
    int i;
    for (i=0; i < 4; i++) {
        vecRes->data[i] = vecA->data[i] + vecB->data[i];
    }
}

void mat4f_mul(Mat4f *matRes, Mat4f *matA, Mat4f *matB) {
    int i, j, k;
    double temp_array[4][4];
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            temp_array[i][j] = 0;
            for (k=0; k<4; k++) {
                temp_array[i][j] += matA->data[i][k] * matB->data[k][j];
            }
        }
    }
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            matRes->data[i][j] = temp_array[i][j];
        }
    }
}

void mat4f_mulf(Mat4f *matrix, float scalar) {
    int i, j;
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++){
            matrix->data[i][j] *= scalar;
        }
    }
}

void mat4f_print(Mat4f *matrix) {
    int i, j;
    float data;
    for (i=0; i<4; i++) {
        printf("[");
        for (j=0; j<4; j++) {
            data = matrix->data[i][j];
            if (data < 0.0f) {
                printf(" %f",data);
            } else {
                printf("  %f",data);
            }
        }
        printf(" ]\n");
    }
    printf("\n");
}

GLvoid *mat4f_pointer(Mat4f *matrix) {
    return (GLvoid*)(matrix->data[0]);
}

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

void mat4f_translate_vec3f(Mat4f *matrix, Vec3f *vector) {
    mat4f_translate(matrix, vector->data[0],
                            vector->data[1],
                            vector->data[2]);
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

int vec3f_allocate(Vec3f **vector) {
    *vector = malloc(sizeof(struct Vec3f));
    if (!*vector) {
        return EXIT_FAILURE;
    }
    int i;
    for(i=0; i<3; i++) {
        (*vector)->data[i] = 0;
    }
    return EXIT_SUCCESS;
}

void vec3f_set(Vec3f *vector, GLfloat x, GLfloat y, GLfloat z) {
    vector->data[0] = x;
    vector->data[1] = y;
    vector->data[2] = z;
}

void vec3f_sub(Vec3f *result, Vec3f *vecA, Vec3f *vecB) {
    int i;
    for(i = 0; i<3; i++) {
        result->data[i] = vecA->data[i] - vecB->data[i];
    }
}

void vec3f_normalize(Vec3f* result, Vec3f *vector) {
    int i;
    float sum, temp;
    sum = 0;
    for(i=0; i<3; i++) {
        temp = vector->data[i];
        sum += temp*temp;
    }
    sum = sqrtf(sum);
    for(i=0; i<3; i++) {
        result->data[i] = (vector->data[i])/sum;
    }
}

void vec3f_cross(Vec3f* result, Vec3f *vecA, Vec3f *vecB) {
    float temp[3], pos, neg;
    int i;
    for (i=0; i<3; i++) {
        pos = vecA->data[(i+1)%3]*vecB->data[(i+2)%3];
        neg = vecA->data[(i+2)%3]*vecB->data[(i+1)%3];
        temp[i] = pos-neg;
    }
    for (i=0; i<3; i++) {
        result->data[i] = temp[i];
    }
}

void vec3f_print(Vec3f* vector) {
    printf("[ ");
    int i;
    for(i=0; i<2; i++) {
        printf("%f, ",vector->data[i]);
    }
    printf("%f ]\n",vector->data[2]);
    printf("\n");
}

void mat4f_look_at(Mat4f *result, Vec3f *camera_pos, Vec3f *camera_target,
                   Vec3f *camera_up) {

    Vec3f *camera_direction, *camera_right, *crossed_up;
    Mat4f *unity, *right_up_down;

    vec3f_allocate(&camera_direction);
    vec3f_allocate(&camera_right);
    vec3f_allocate(&crossed_up);

    mat4f_allocate(&unity);
    mat4f_allocate(&right_up_down);

    vec3f_sub(camera_direction, camera_pos, camera_target);
    vec3f_normalize(camera_direction, camera_direction);

    vec3f_cross(camera_right, camera_up, camera_direction);
    vec3f_normalize(camera_right, camera_right);
    vec3f_cross(crossed_up, camera_direction, camera_right);

    mat4f_unity(unity);
    mat4f_translate(unity,
                    -camera_pos->data[0],
                    -camera_pos->data[1],
                    -camera_pos->data[2]);
    int i;
    for(i=0; i<3; i++) {
        right_up_down->data[0][i] = camera_right->data[i];
        right_up_down->data[1][i] = crossed_up->data[i];
        right_up_down->data[2][i] = camera_direction->data[i];
    }
    right_up_down->data[3][3] = 1.0f;
    mat4f_mul(result, right_up_down, unity);

    free(camera_direction);
    free(camera_right);
    free(crossed_up);
    free(unity);
}

int load_model(float **vertices, int *total_vertices) {
    const char *filename = "models/coffee_pot.obj";
    //const char *filename = "models/box.obj";
    FILE *file_pointer = fopen(filename, "rb");
    if (!file_pointer) {
        fprintf(stderr, "No such file: %s\n",filename);
        return EXIT_FAILURE;
    }
    int BUFFER_SIZE = 256;
    char linebuffer[BUFFER_SIZE];
    int i;

    enum {v, vt, vn, f, num_prefixes};
    const char *prefixes[num_prefixes] = {"v","vt","vn","f"};
    char *token;
    char separator = ' ';
    char strtok_separator[] = " ";
    char *save_first;

    int prefix_total[num_prefixes] = {0};
    int prefix_sizes[num_prefixes] = {3, 2, 3, 4};
    int face_size = 3;

    while(fgets(linebuffer, BUFFER_SIZE, file_pointer) != NULL) {
        token = strtok(linebuffer, strtok_separator);
        while (token != NULL) {
            for (i=0; i<num_prefixes; i++) {
                if (strcmp(prefixes[i], token) == 0) {
                    prefix_total[i]++;
                }
            }
            break;
        }
    }

    float ***data;
    int ***faces;
    data = malloc(sizeof(float*)*num_prefixes);
    faces = malloc(sizeof(int*)*prefix_total[f]);
    for (i=0; i<num_prefixes; i++) {
        if (i == f) {
            for (int j=0; j<prefix_total[i]; j++) {
                faces[j] = malloc(sizeof(int*)*prefix_sizes[i]);
                for (int k=0; k<prefix_sizes[i]; k++) {
                    faces[j][k] = malloc(sizeof(int)*face_size);
                }
            }
        } else {
            data[i] = malloc(sizeof(float*)*prefix_total[i]);
            for (int j=0; j<prefix_total[i]; j++) {
                data[i][j] = malloc(sizeof(float)*prefix_sizes[i]);
            }
        }
    }
    int current_vertex[num_prefixes] = {0};
    int parameter_flag = -1;
    char *face_token;
    char face_separator = '/';
    char *save_second;

    rewind(file_pointer);

    while(fgets(linebuffer, BUFFER_SIZE, file_pointer) != NULL) {
        //printf("linebuffer: %s\n",linebuffer);
        token = token_parser(linebuffer, separator, &save_first);
        //printf("Token is = %s\n",token);
        parameter_flag = -1;
        while (token != NULL) {
            for (i=0; i<num_prefixes; i++) {
                //printf("prefix: %s, token: %s\n", prefixes[i], token);
                if (strcmp(prefixes[i], token) == 0) {
                    parameter_flag = i;
                    break;
                }
            }
            //printf("continuing with flag: %d\n",parameter_flag);
            if (parameter_flag < 0) {
                break;
            }
            for (i=0; i<prefix_sizes[parameter_flag]; i++) {
                token = token_parser(NULL, separator, &save_first);
                //printf("token in for: %s\n",token);
                int current_vert = current_vertex[parameter_flag];
                if (parameter_flag != f) {
                    float value = strtof(token, NULL);
                    //printf("value: %f\n",value);
                    //printf("Trying to write to data[%d][%d][%d]\n",parameter_flag,current_vert,i);
                    //printf("total vn: %d\n",prefix_total[2]);
                    //printf("total vn: %d\n",prefix_total[vn]);
                    data[parameter_flag][current_vert][i] = value;
                } else {
                    //printf("token: %s\n",token);
                    face_token = token_parser(token, face_separator, &save_second);
                    for (int j=0; j<face_size; j++) {
                        //printf("face_number: %s\n",face_token);
                        faces[current_vert][i][j] = ((int)strtof(face_token, NULL))-1;
                        face_token = token_parser(NULL, face_separator, &save_second);
                    }
                }
            }
            current_vertex[parameter_flag]++;
            break;
        }
    }

    fclose(file_pointer);

    for (i=0; i<num_prefixes; i++) {
        current_vertex[i] = 0;
    }

    //array = malloc(sizeof(float)*prefix_total[f]*8);
    int array_size = prefix_total[f]*8*(face_size+1);
    *vertices = malloc(sizeof(float)*array_size);
    //float array[array_size];
    int index = 0;
    int num_vertices = 0;
    int vert_index = 0;
    for (i=0; i<prefix_total[f]; i++) {
        for (int j=0; j<prefix_sizes[f]; j++) {
            for (int k=0; k<face_size; k++) {
                index = faces[i][j][k];
                for(int l=0; l<prefix_sizes[k]; l++) {
                    if (index < 0) {
                        (*vertices)[vert_index] = 0.0f;
                        //array[vert_index] = 0.0f;
                        //printf("0.0f, ");
                    } else {
                        (*vertices)[vert_index] = data[k][index][l];
                        //array[vert_index] = data[k][index][l];
                        //printf("%ff, ",data[k][index][l]);
                    }
                    //printf("vert_index: %d\n",vert_index);
                    //} else {
                    //}
                    vert_index++;
                }
            }
            num_vertices++;
            //printf("\n");
        }
    }
    //printf("%d\n",array_size);
    //printf("%d\n",vert_index);
    //printf("%d\n",num_vertices);

    //for(i=0; i<array_size; i++) {
    //    if (i != 0 && i%8 == 0) {
    //        printf("\n");
    //    }
    //    //printf("%f, ",array[i]);
    //    printf("%f, ",(*vertices)[i]);
    //}

    *total_vertices = num_vertices;

    for (i=0; i<num_prefixes-1; i++) {
        for (int j=0; j<prefix_total[i]; j++) {
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);

    for (i=0; i<prefix_total[f]; i++) {
        for (int j=0; j<prefix_sizes[f]; j++) {
            free(faces[i][j]);
        }
        free(faces[i]);
    }
    free(faces);
    return(EXIT_SUCCESS);
}

char *token_parser(char *string, char delimiter, char** save_pointer) {
    char *string_pointer;
    if (string == NULL) {
        string = *save_pointer;
    }
    string_pointer = string;
    while(*string_pointer != '\0' && *string_pointer != delimiter) {
        string_pointer++;
    }
    if (&string_pointer == save_pointer) {
        return NULL;
    }
    if (*string_pointer == '\0') {
        *save_pointer = string_pointer;
    } else {
        *save_pointer = string_pointer+1;
    }
    *string_pointer = '\0';
    return string;
}

#endif
