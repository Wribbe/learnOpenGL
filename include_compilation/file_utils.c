#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#include "file_utils.h"

#include <GL/glew.h>
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

char* read_shader(const char* filename) {
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


#endif
