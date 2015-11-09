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

#endif
