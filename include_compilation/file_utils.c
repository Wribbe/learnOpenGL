#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <SOIL.h>
#include <math.h>

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
                                           SOIL_LOAD_RGB);
    if (!image) {
        fprintf(stderr, "Could not load image: %s.\n",filename);
        fprintf(stderr, "result: %s\n",SOIL_last_result());
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
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
        return NULL;
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
    double temp_array[4];
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            temp_array[j] = 0;
            for (k=0; k<4; k++) {
                temp_array[j] += matA->data[i][k] * matB->data[k][j];
            }
        }
        for (j=0; j<4; j++) {
            matRes->data[i][j] = temp_array[j];
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
#endif
