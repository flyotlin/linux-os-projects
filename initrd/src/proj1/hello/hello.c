#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mm.h>

void print_task_info(void);
void print_task_mm(void);

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

SYSCALL_DEFINE0(hello, struct seg_info __user *, info)
{
    int size = sizeof(struct seg_info);
    struct seg_info* info_buf = (struct seg_info*) kmalloc(size, GFP_KERNEL);

    if (info_buf == NULL) {
        return -ENOMEM;
    }

    if (copy_from_user(info_buf, info, size)) {
        return -EFAULT;
    }

    info_buf->pid = current->pid;
    info_buf->tgid = current->tgid;
    info_buf->total_vm = current->mm->total_vm;
    info_buf->start_code = current->mm->start_code;
    info_buf->end_code = current->mm->end_code;
    info_buf->start_data = current->mm->start_data;
    info_buf->end_data = current->mm->end_data;
    info_buf->start_brk = current->mm->start_brk;
    info_buf->brk = current->mm->brk;
    info_buf->start_stack = current->mm->start_stack;

    if (copy_to_user(info, info_buf, size)) {
        return -EFAULT;
    }

    print_task_info();
    print_task_mm();

    kfree(info_buf);

    return 0;
}

void print_task_info(void)
{
    long tgid = (long) current->tgid;
    long pid = (long) current->pid;
    printk("[tgid, pid]: [%ld, %ld]\n", tgid, pid);
}

void print_task_mm(void)
{
    struct mm_struct *mm = current->mm;
    printk("code: [%016lx ~ %016lx]\n", mm->start_code, mm->end_code);
    printk("data: [%016lx ~ %016lx]\n", mm->start_data, mm->end_data);
    printk("heap: [%016lx ~ %016lx]\n", mm->start_brk, mm->brk);
    printk("stack: [%016lx ~]\n", mm->start_stack);
}
