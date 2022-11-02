#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <sys/types.h>


// Struct to retrieve segment information
struct thread_segment_info {
    pid_t pid, tgid;
    unsigned long total_vm;     // total pages mapped
    unsigned long start_code;
    unsigned long end_code;
    unsigned long start_data;
    unsigned long end_data;
    unsigned long start_brk;
    unsigned long brk;
    unsigned long start_stack;
};

void* child(void*);
void show_info(struct thread_segment_info *);


int main()
{
    // Try to initialize data and heap segment
    struct thread_segment_info *info = (struct thread_segment_info) malloc(sizeof(struct thread_segment_info));
    int x = 3;
    int *y = (int *) malloc(sizeof(int));
    *y = 4;

    long int ret = syscall(323, getpid(), info);

    // Create thread 1
    pthread_t thread_1;
    pthread_create(&thread_1, NULL, child, "Child");

    // Create thread 2
    pthread_t thread_2;
    pthread_create(&thread_2, NULL, child, "Child");

    // Wait 2 threads finish
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    return 0;
}

void* child(void* data)
{
    // Try to initialize data and heap segment
    struct thread_segment_info *info = (struct thread_segment_info) malloc(sizeof(struct thread_segment_info));
    int x = 3;
    int *y = (int *) malloc(sizeof(int));
    *y = 4;

    // Call syscall to get segment information
    long int ret = syscall(323, getpid(), info);

    printf("Thread returend %ld\n", ret);
    printf("Thread: %lx, %x\n", pthread_self(), getpid());

    pthread_exit(0);
}

void show_info(struct thread_segment_info *info)
{
    printf("Task [%ld, %ld]:\n", info->pid, info->tgid);
    printf("total vm: %ld\n", info->total_vm);
    printf("data segment: [%ld ~ %ld]\n", info->start_data, info->end_data);
    printf("code segment: [%ld ~ %ld]\n", info->start_code, info->end_code);
    printf("brk segment: [%ld ~ %ld]\n", info->start_brk, info->brk);
    printf("stack segment: [%ld ~]\n", info->start_stack);
    printf("=========\n\n");
}
