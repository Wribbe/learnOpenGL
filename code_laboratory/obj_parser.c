#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void load_model(float** vertices, int* size) {
    //const char *filename = "coffee_pot.obj";
    const char *filename = "box.obj";
    FILE *file_pointer = fopen(filename, "rb");
    if (!file_pointer) {
        fprintf(stderr, "No such file: %s\n",filename);
        return EXIT_FAILURE;
    }
    int BUFFER_SIZE = 256;
    char linebuffer[BUFFER_SIZE], token_buffer[4];
    int current_char, i;

    enum {v, vt, vn, f, num_prefixes};
    char *prefixes[num_prefixes] = {"v","vt","vn","f"};
    char *token;
    char separators[] = " ";

    int prefix_total[num_prefixes] = {0};
    int prefix_sizes[num_prefixes] = {3, 2, 3, 4};
    int face_size = 3;

    while(fgets(linebuffer, BUFFER_SIZE, file_pointer) != NULL) {
        token = strtok(linebuffer, separators);
        while (token != NULL) {
            for (i=0; i<num_prefixes; i++) {
                if (strcmp(prefixes[i], token) == 0) {
                    prefix_total[i]++;
                }
            }
            break;
        }
    }

    for (i=0; i<num_prefixes; i++) {
        printf("%s: %d\n",prefixes[i],prefix_total[i]);
    }

    rewind(file_pointer);

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
    const char *face_separator = "/";
    char *save_first, *save_second;

    while(fgets(linebuffer, BUFFER_SIZE, file_pointer) != NULL) {
        token = strtok_r(linebuffer, separators, &save_first);
        parameter_flag = -1;
        while (token != NULL) {
            for (i=0; i<num_prefixes; i++) {
                if (strcmp(prefixes[i], token) == 0) {
                    parameter_flag = i;
                    break;
                }
            }
            if (parameter_flag < 0) {
                break;
            }
            for (i=0; i<prefix_sizes[parameter_flag]; i++) {
                token = strtok_r(NULL, separators, &save_first);
                int current_vert = current_vertex[parameter_flag];
                if (parameter_flag != f) {
                    data[parameter_flag][current_vert][i] = strtof(token, NULL);
                } else {
                    //printf("token: %s\n",token);
                    face_token = strtok_r(token, face_separator, &save_second);
                    for (int j=0; j<face_size; j++) {
                        //printf("face_number: %s\n",face_token);
                        faces[current_vert][i][j] = ((int)strtof(face_token, NULL))-1;
                        face_token = strtok_r(NULL, face_separator, &save_second);
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

    int index = 0;
    int num_vertices = 0;
    for (i=0; i<prefix_total[f]; i++) {
        for (int j=0; j<prefix_sizes[f]; j++) {
            for (int k=0; k<face_size; k++) {
                index = faces[i][j][k];
                for(int l=0; l<prefix_sizes[k]; l++) {
                    if (index < 0) {
                        printf("0.0f, ");
                    } else {
                        printf("%ff, ",data[k][index][l]);
                    }
                }
            }
            num_vertices++;
            //printf("\n");
        }
    }
    //printf("%d\n",vertices);

    *vertices = malloc(sizeof(float)*num_vertices);
    *size = num_vertices;

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
}
