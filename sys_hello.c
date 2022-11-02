#include <linux/kernel.h>
#include <linux/sched.h>

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

/**
 * Arguments:
 *      pid: pid in userspace (getpid())
 *      tid: tid in userspace (gettid())
*/
asmlinkage long sys_hello(pid_t pid, pid_t tid, struct thread_segment_info *info)
{
    struct task_struct *process, *thread;
    if (!pid || pid < 0) {
        printk("pid not given properly!\n");
        return -1;
    }
    printk("Start doing sys_hello: [%ld, %ld]...\n", pid, tid);

    for_each_process(process) {
        if (process->tgid != pid) {
            continue;
        }
        for_each_thread(process, thread) {
            if (thread->pid != tid) {
                continue;
            }
            printk("Hit one thread!\n");
            // Update and fill in segment information
            info->pid = thread->pid;
            info->tgid = thread->tgid;

            info->total_vm = thread->mm->total_vm;
            info->start_code = thread->mm->start_code;
            info->end_code = thread->mm->end_code;
            info->start_data = thread->mm->start_data;
            info->end_data = thread->mm->end_data;
            info->start_brk = thread->mm->start_brk;
            info->brk = thread->mm->brk;
            info->start_stack = thread->mm->start_stack;

            // Try to printk some information in dmesg
            // TODO: a virtual -> physical mechanism (by looking up in page table)
            printk("[%ld, %ld]: %ld, %ld, %ld, %ld, %ld\n", pid, tid, thread->mm->start_code, thread->mm->end_code, thread->mm->start_data, thread->mm->end_data, thread->mm->start_stack);
            return 0;
        }
    }
    printk("Hit no thread!\n");
    return 0;
}
