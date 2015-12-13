#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(void) {
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

    //long accumulated_reads = 0;

    //fseek(file_pointer, 0, SEEK_END);
    //long file_size = ftell(file_pointer);
    //rewind(file_pointer);
    //printf("File size: %lu\n",file_size);

    //while(accumulated_reads < file_size) {
    //    fread(&linebuffer[0], BUFFER_SIZE, 1, file_pointer);
    //    accumulated_reads += BUFFER_SIZE;
    //    for (i=0; i<BUFFER_SIZE; i++) {
    //        current_char = linebuffer[i];
    //        if (current
    //    }
    //}
    //size_t temp_buffer_size = fread(&linebuffer[0], BUFFER_SIZE, 1, file_pointer);
    //printf("Last read was: %zu long.\n", temp_buffer_size);
    //

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
    for (i=0; i<num_prefixes; i++) {
        current_vertex[i] = 0;
    }

    int index = 0;
    int vertices = 0;
    for (i=0; i<prefix_total[f]; i++) {
        for (int j=0; j<prefix_sizes[f]; j++) {
            for (int k=0; k<face_size; k++) {
                index = faces[i][j][k];
                for(int l=0; l<prefix_sizes[k]; l++) {
                    if (index < 0) {
                        //printf("0.0f, ");
                        printf("0.0f, ");
                    } else {
                        //printf("data_type: %d ",k);
                        //printf("data_row_of_type: %d ",index);
                        //printf("element_of_data_row: %d ",l);
                        printf("%.2ff, ",data[k][index][l]);
                    }
                }
            }
            vertices++;
            printf("\n");
        }
    }
    printf("%d\n",vertices);

    //for (i=0; i<num_prefixes-1; i++) {
    //    for(int j=0; j<prefix_total[i]; j++) {
    //        for(int k=0; k<prefix_sizes[i]; k++) {
    //            printf("%f,",data[i][j][k]);
    //        }
    //        printf("\n");
    //    }
    //    printf("\n");
    //}
    //bool prefix_left = true;
    //while(prefix_left) {
    //    for (i=0; i<num_prefixes-1; i++) {
    //        for (int k=0; k<prefix_sizes[i]; k++) {
    //            if (prefix_total[i] == 0 || current_vertex[i] >= prefix_total[i]) {
    //                printf("-0f,");
    //            } else {
    //                printf("%.2ff, ",data[i][current_vertex[i]][k]);
    //            }
    //        }
    //        current_vertex[i]++;
    //    }
    //    printf("\n");
    //    prefix_left = false;
    //    for (i=0; i<num_prefixes-1; i++) {
    //        if (current_vertex[i] < prefix_total[i]) {
    //            prefix_left = true;
    //            continue;
    //        }
    //    }
    //}
    fclose(file_pointer);
}
