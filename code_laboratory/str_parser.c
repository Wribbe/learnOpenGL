#include <stdlib.h>
#include <stdio.h>

char *parse_string(char *string, char delimiter, char** save_pointer) {
    if (*save_pointer == '\0') {
        printf("this?\n");
        return NULL;
    }
    char *second_pointer, *original_save;
    if (string == NULL) {
        second_pointer = *save_pointer;
        original_save = *save_pointer;
    } else {
        second_pointer = string;
    }
    while(*second_pointer != '\0' || *second_pointer != delimiter) {
        second_pointer++;
    }
    if (*second_pointer == '\0') {
        *save_pointer = second_pointer;
        return original_save;
    }
    *second_pointer= '\0';
    *save_pointer = second_pointer+1;
    if (string == NULL) {
        return original_save;
    }
    return string;
}

char *parse_other(char *string, char delimiter, char** save_pointer) {
    char* string_pointer;
    if (string == NULL) {
        string = *save_pointer;
    }
    string_pointer = string;
    while(*string_pointer != '\0' && *string_pointer != delimiter) {
        string_pointer++;
    }
    *string_pointer = '\0';
    *save_pointer = string_pointer+1;
    return string;
}

char *token_parser(char *string, char delimiter, char** save_pointer) {
    char *string_pointer;
    if (string == NULL) {
        string = *save_pointer;
    }
    string_pointer = string;
    while(*string_pointer != '\0' && *string_pointer != delimiter) {
        string_pointer++;
    }
    if (string_pointer == *save_pointer) {
        return NULL;
    }
    if (*string_pointer == '\0') {
        *save_pointer = string_pointer;
    } else {
        *save_pointer = string_pointer+1;
    }
    *string_pointer = '\0';
    return string;
}

int main(void) {
    char string[] = "This.is is.a a.second test.parsing";
    char *save, *save2;
    char *token, *sub_token;
    //printf("String: %s\n",string);
    //token = parse_string(string, ' ', &save);
    //printf("token: %s\n",token);
    //while(token != NULL) {
    //    printf("token: %s\n",token);
    //    token = parse_string(NULL, ' ', &save);
    //}
    //

    token = token_parser(string, ' ', &save);
    printf("%s\n", token);
    token = token_parser(string, ' ', &save);
    printf("%s\n", token);
    token = token_parser(string, ' ', &save);
    printf("%s\n", token);
    //while(token != NULL) {
    //    printf("%s\n", token);
    //    sub_token = token_parser(token, '.', &save2);
    //    while(sub_token != NULL) {
    //        printf("  %s\n",sub_token);
    //        sub_token = token_parser(NULL, '.', &save2);
    //    }
    //    token = token_parser(NULL, ' ', &save);
    //}
    return 0;
}
