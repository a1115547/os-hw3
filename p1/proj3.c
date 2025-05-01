#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N_LIST 10

int sum;  // shared by the thread
int list[N_LIST] = { 7, 12, 19, 3, 18, 4, 2, 6, 15, 8 };
int copied_list[N_LIST];

// thread function
void* runner(void* param);

// list copy function
int* listncopy(int* dst, int* src, int n) {
    for (int i = 0; i < n; i++)
        dst[i] = src[i];
    return dst;
}

int main(int argc, char* argv[])
{
    pthread_t tid;           // thread ID
    pthread_attr_t attr;     // thread attributes

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <upper>\n", argv[0]);
        return -1;
    }

    // Step 1: Copy list
    listncopy(copied_list, list, N_LIST);
    printf("a1115547 Copied List:");
    for (int i = 0; i < N_LIST; i++)
        printf(" %d", copied_list[i]);
    printf("\n");

    // Step 2: Set thread and create runner
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, runner, argv[1]);
    pthread_join(tid, NULL);

    // Step 3: Print sum result with student ID
    printf("a1115547 Sum = %d\n", sum);
    return 0;
}

// thread adds 1 + 2 + ... + upper
void* runner(void* param)
{
    int upper = atoi(param);
    sum = 0;
    for (int i = 1; i <= upper; i++)
        sum += i;
    pthread_exit(0);
}
