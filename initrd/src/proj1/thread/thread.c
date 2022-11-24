#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define SYSCALL_HELLO 449

// Struct to retrieve segment information
struct seg_info {
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

void* child1(void*);
void* child2(void*);
void show_info(struct seg_info *);

int main()
{
    struct seg_info* info = (struct seg_info*) malloc(sizeof(struct seg_info));
    long int ret = syscall(SYSCALL_HELLO, info);
    show_info(info);

    // Create thread 1
    pthread_t thread_1;
    pthread_create(&thread_1, NULL, child1, "Child");

    // Create thread 2
    pthread_t thread_2;
    pthread_create(&thread_2, NULL, child2, "Child");

    // Wait 2 threads finish
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);

    return 0;
}

void* child1(void* data)
{
    struct seg_info* info = (struct seg_info*) malloc(sizeof(struct seg_info));
    long int ret = syscall(SYSCALL_HELLO, info);

    show_info(info);
    pthread_exit(0);
}

void* child2(void* data)
{
    struct seg_info* info = (struct seg_info*) malloc(sizeof(struct seg_info));
    long int ret = syscall(SYSCALL_HELLO, info);

    show_info(info);
    pthread_exit(0);
}

void show_info(struct seg_info *info)
{
    printf("Task [%ld, %ld]:\n", info->pid, info->tgid);
    printf("total vm: %ld\n", info->total_vm);
    printf("data segment: [%ld ~ %ld]\n", info->start_data, info->end_data);
    printf("code segment: [%ld ~ %ld]\n", info->start_code, info->end_code);
    printf("brk segment: [%ld ~ %ld]\n", info->start_brk, info->brk);
    printf("stack segment: [%ld ~]\n", info->start_stack);
    printf("=========\n\n");
}