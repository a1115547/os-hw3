#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define N_LIST 10

int original_list[] = { 7, 12, 19, 3, 18, 4, 2, 6, 15, 8 };
int sorted_list[N_LIST];   // after sorting
int merged_list[N_LIST];   // final result

struct timeval start, end;

#define usec_elapsed(s, e) (1000000 * ((e).tv_sec - (s).tv_sec) + ((e).tv_usec - (s).tv_usec))

typedef struct {
    char id[32];
    int* data;
    int size;
} thread_param;

// Function to print list
void print_list(const char* id, const char* label, int* list, int size) {
    printf("%s %s:", id, label);
    for (int i = 0; i < size; i++) {
        printf(" %d", list[i]);
    }
    printf("\n");
}

// Copy list
int* listncopy(int* dst, int* src, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
    return dst;
}

// Compare function for qsort
int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// do_sort(): 排序 thread 會呼叫這裡
void* do_sort(void* arg) {
    thread_param* param = (thread_param*)arg;
    print_list(param->id, "Sub-Old", param->data, param->size);

    gettimeofday(&start, NULL);
    qsort(param->data, param->size, sizeof(int), compare);
    gettimeofday(&end, NULL);

    print_list(param->id, "Sub-New", param->data, param->size);
    printf("%s spent %ld usec\n", param->id, usec_elapsed(start, end));

    pthread_exit(0);
}

// do_merge(): 合併 thread 會呼叫這裡
void* do_merge(void* arg) {
    thread_param* param = (thread_param*)arg;

    int* a = &sorted_list[0];
    int* b = &sorted_list[N_LIST / 2];
    int i = 0, j = 0, k = 0;

    gettimeofday(&start, NULL);
    while (i < N_LIST / 2 && j < N_LIST / 2) {
        if (a[i] <= b[j]) {
            merged_list[k++] = a[i++];
        }
        else {
            merged_list[k++] = b[j++];
        }
    }
    while (i < N_LIST / 2) merged_list[k++] = a[i++];
    while (j < N_LIST / 2) merged_list[k++] = b[j++];
    gettimeofday(&end, NULL);

    print_list(param->id, "Merged", merged_list, N_LIST);
    printf("%s spent %ld usec\n", param->id, usec_elapsed(start, end));

    pthread_exit(0);
}

int main() {
    const char* student_id = "a1115547";

    pthread_t tid_sort1, tid_sort2, tid_merge;
    thread_param p1, p2, pm;

    listncopy(sorted_list, original_list, N_LIST); // Step 1
    print_list(student_id, "All-Old", sorted_list, N_LIST);

    // Setup sorting thread 1
    snprintf(p1.id, sizeof(p1.id), "%s#0", student_id);
    p1.data = &sorted_list[0];
    p1.size = N_LIST / 2;

    // Setup sorting thread 2
    snprintf(p2.id, sizeof(p2.id), "%s#1", student_id);
    p2.data = &sorted_list[N_LIST / 2];
    p2.size = N_LIST / 2;

    // Create sorting threads
    pthread_create(&tid_sort1, NULL, do_sort, &p1);
    pthread_create(&tid_sort2, NULL, do_sort, &p2);

    pthread_join(tid_sort1, NULL);
    pthread_join(tid_sort2, NULL);

    // Setup merge thread
    snprintf(pm.id, sizeof(pm.id), "%s#M", student_id);
    pm.data = merged_list;
    pm.size = N_LIST;

    // Create merge thread
    pthread_create(&tid_merge, NULL, do_merge, &pm);
    pthread_join(tid_merge, NULL);

    print_list(student_id, "All-New", merged_list, N_LIST);

    return 0;
}
