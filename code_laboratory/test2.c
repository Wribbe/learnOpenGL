#include <stdlib.h>
#include <stdio.h>

void main(void) {
    int first = 3;
    int num_arrays = 5;
    int array_size = 4;
    int i;
    float ***data;
    data = malloc(sizeof(float*)*first);
    for (i=0; i<first; i++) {
        data[i] = malloc(sizeof(float*)*num_arrays);
        for (int j=0; j<num_arrays; j++) {
            data[i][j] = malloc(sizeof(float)*array_size);
            for (int k=0; k<array_size; k++) {
                data[i][j][k] = 1.0f;
            }
        }
    }

    for (i=0; i<first; i++) {
        printf("data[%d]: ",i);
        for (int j=0; j<num_arrays; j++) {
            printf("{");
            for (int k=0; k<array_size; k++) {
                printf("%f,",data[i][j][k]);
            }
            printf("},");
        }
        printf("\n");
    }
    for (i=0; i<first; i++) {
        for (int j=0; j<num_arrays; j++) {
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);
}
