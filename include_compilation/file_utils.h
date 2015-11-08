#ifndef FILE_UTILS_H_INCLUDED
#define FILE_UTILS_H_INCLUDED

#include <GL/glew.h>

int info_log_size;
char* info_log;
GLint success;

char *read_file(const char* filename);

#endif
