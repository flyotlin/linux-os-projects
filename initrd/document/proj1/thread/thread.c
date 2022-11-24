#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define SYSCALL_HELLO 449


void* child1(void*);
void* child2(void*);
int global_data = 10;

int main()
{
    int local_var = 3;
    long int ret = syscall(SYSCALL_HELLO, &local_var);
    printf("System call sys_hello returend %ld\n", ret);

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
    int local_var = 4;
    long int ret = syscall(SYSCALL_HELLO, &local_var);
    printf("System call sys_hello returend %ld\n", ret);

    pthread_exit(0);
}

void* child2(void* data)
{
    int local_var = 5;
    long int ret = syscall(SYSCALL_HELLO, &local_var);
    printf("System call sys_hello returend %ld\n", ret);

    pthread_exit(0);
}
