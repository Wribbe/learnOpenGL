#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void) {
//    char line[] = "This.is the.only string.that should.be separated";
//    char separator1[] = " ";
//    char separator2[] = ".";
//
//    char *first_token, *second_token;
//    char *first_save, *second_save;
//
//    printf("%s\n",line);
//    first_token = strtok_r(line, separator1, &first_save);
//    while(first_token != NULL) {
//        printf("First_token: %s\n",first_token);
//        second_token = strtok_r(first_token, separator2, &second_save);
//        first_token = strtok_r(NULL, separator1, &first_save);
//        while(second_token != NULL) {
//            printf("  Second_token: %s\n",second_token);
//            second_token = strtok_r(NULL, separator2, &second_save);
//        }
//    }

    int ***array;
    int first = 3;
    int second = 4;
    int third = 5;
    array = malloc(sizeof(int*)*first);
    for (int i=0; i<first; i++) {
        array[i] = malloc(sizeof(int*)*second);
        for(int j=0; j<second; j++) {
            array[i][j] = malloc(sizeof(int)*third);
            for(int k=0; k<third; k++) {
                array[i][j][k] = i+j+k;
            }
        }
    }
    for (int i=0; i<first; i++) {
        for (int j=0; j<second; j++) {
            printf("{");
            for (int k=0; k<third; k++) {
                printf("%d, ",array[i][j][k]);
            }
            printf("}");
        }
        printf("\n");
    }
    for (int i=0; i<first; i++) {
        for(int j=0; j<second; j++) {
            free(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
    return 1;
}
