#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/pgtable.h>

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
 * Convert linear address to physical address by page table
 * Return Value:
 *      0: success
 *      -1: fail
*/
int linear_to_physical(struct mm_struct *mm, unsigned long linear, unsigned long *physical)
{
    unsigned long page_addr, page_offset;
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    // handle pgd
    pgd = pgd_offset(mm, linear);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        printk("pgd: Failed to convert from linear to physical\n");
        return -1;
    }

    // handle pud
    pud = pud_offset(pgd, linear);
    if (pud_none(*pud) || pud_bad(*pud)) {
        printk("pud: Failed to convert from linear to physical\n");
        return -1;
    }

    // handle pmd
    pmd = pmd_offset(pud, linear);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        printk("pmd: Failed to convert from linear to physical\n");
        return -1;
    }

    // handle pte
    pte = pte_offset_kernel(pmd, linear);
    if (pte_none(*pte)) {
        printk("pte: Failed to convert from linear to physical\n");
        return -1;
    }

    page_addr = pte_val(*pte) & PAGE_MASK;
    page_offset = linear & ~PAGE_MASK;
    *physical = page_addr | page_offset;
    printk("Convert from linear to physical successfully\n");
    return 0;
}

/**
 * Convert linear addresses in thread_segment_info to physical address
*/
void convert_info_to_physical(struct mm_struct *mm, struct thread_segment_info *info)
{
    int success = 1;
    unsigned long tmp_addr;

    // start_code
    tmp_addr = info->start_code;
    if (0 != linear_to_physical(mm, tmp_addr, &info->start_code)) {
        success = 0;
        goto Exit;
    }
    // end_code
    tmp_addr = info->end_code;
    if (0 != linear_to_physical(mm, tmp_addr, &info->end_code)) {
        success = 0;
        goto Exit;
    }
    // start_data
    tmp_addr = info->start_data;
    if (0 != linear_to_physical(mm, tmp_addr, &info->start_data)) {
        success = 0;
        goto Exit;
    }
    // end_data
    tmp_addr = info->end_data;
    if (0 != linear_to_physical(mm, tmp_addr, &info->end_data)) {
        success = 0;
        goto Exit;
    }
    // start_brk
    tmp_addr = info->start_brk;
    if (0 != linear_to_physical(mm, tmp_addr, &info->start_brk)) {
        success = 0;
        goto Exit;
    }
    // brk
    tmp_addr = info->brk;
    if (0 != linear_to_physical(mm, tmp_addr, &info->brk)) {
        success = 0;
        goto Exit;
    }
    // start_stack
    tmp_addr = info->start_stack;
    if (0 != linear_to_physical(mm, tmp_addr, &info->start_stack)) {
        success = 0;
        goto Exit;
    }

Exit:
    if (success) {
        printk("Successfully convert info to physical\n");
    } else {
        printk("Failed to convert info to physical\n");
    }
    return;
}

/**
 * Arguments:
 *      pid: pid in userspace (getpid())
 *      tid: tid in userspace (gettid())
*/
asmlinkage long sys_hello(int is_physical, pid_t pid, pid_t tid, struct thread_segment_info *info)
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

            if (is_physical) {
                convert_info_to_physical(thread->mm, info);
            }

            // Try to printk some information in dmesg
            printk("[%ld, %ld]: %ld, %ld, %ld, %ld, %ld\n", pid, tid, thread->mm->start_code, thread->mm->end_code, thread->mm->start_data, thread->mm->end_data, thread->mm->start_stack);
            return 0;
        }
    }
    printk("Hit no thread!\n");
    return 0;
}
