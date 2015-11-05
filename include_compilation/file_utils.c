#include <stdio.h>
#include <stdlib.h>

char *read_file(char* filename) {
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
