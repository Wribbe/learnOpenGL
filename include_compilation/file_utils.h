#ifndef FILE_UTILS_H_INCLUDED
#define FILE_UTILS_H_INCLUDED

#include <GL/glew.h>

#define info_log_size 512

char info_log[info_log_size];
GLint success;

char *read_file(const char *filename);
char *read_shader(const char *filename);

GLuint compile_shader(char *vertex_source, GLenum shader_type);
GLuint create_shader_program(char *vertex_source, char *framgent_source);

void load_texture(const char *filename);

typedef struct Matrix4f {
    float data[4][4];
} Mat4f;

typedef struct Vec4f {
    float data[4];
} Vec4f;

/* Helper matrix functions. */
void mat4f_zero(Mat4f *matrix);
void mat4f_zero_col(Mat4f *matrix, int col);
void mat4f_zero_row(Mat4f *matrix, int row);
void mat4f_basic_rotate(Mat4f *matrix, float radians);

/* Useful matrix functions. */
void mat4f_allocate(Mat4f *matrix);

void mat4f_scale(Mat4f *matrix, float x, float y, float z);

void mat4f_rotate_x(Mat4f *matrix, float radians);
void mat4f_rotate_y(Mat4f *matrix, float radians);
void mat4f_rotate_z(Mat4f *matrix, float radians);

void mat4f_add(Mat4f *matA, Mat4f *matB, Mat4f *matRes);
void mat4f_mul(Mat4f *matA, Mat4f *matB, Mat4f *matRes);
void mat4f_mulf(Mat4f *matrix, float scalar);

void mat4f_print(Mat4f *matrix);

GLvoid *mat4f_pointer(Mat4f *matrix);
#endif
