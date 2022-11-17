#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>

void print_task_info(void);
void print_task_mm(void);

SYSCALL_DEFINE0(hello)
{
    print_task_info();
    print_task_mm();

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
