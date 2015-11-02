#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct thread_input {
    int index;
} thread_input;

void* print_thread(void *void_input) {
    thread_input *input = (thread_input*)void_input;
    int index = input->index;
    printf("This is a message from thread: %d\n", index);
    return void_input;
}

int main(void) {

    int num_threads = 10;
    thread_input inputs[num_threads];
    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; i++) {
        inputs[i].index = i;
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, print_thread, &inputs[i]);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
