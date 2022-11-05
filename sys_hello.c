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

int linear_to_physical(struct mm_struct *, unsigned long, unsigned long *);
void convert_info_to_physical(struct mm_struct *, struct thread_segment_info *);
void update_info(struct task_struct *, struct thread_segment_info *);
void print_all_vma(pid_t, pid_t, struct mm_struct *);
void print_mm_segments(pid_t, pid_t, struct mm_struct *);


/**
 * System Call for retrieving segment information
 * Arguments:
 *      is_physical: return virtual or physical address
 *      pid: pid in userspace (getpid())
 *      tid: tid in userspace (gettid())
 *      info: get information within one struct task_struct from kernel to user space
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
            // Check if kernel or user thread
            if (thread->mm) {
                printk("User space thread!\n");
            } else {
                printk("Kernel space thread!\n");
            }

            update_info(thread, info);
            if (is_physical) {
                convert_info_to_physical(thread->mm, info);
            }
            print_all_vma(thread->pid, thread->tgid, thread->mm);
            print_mm_segments(thread->pid, thread->tgid, thread->mm);
            return 0;
        }
    }
    printk("Hit no thread!\n");
    return 0;
}

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
    printk("", mm->pgd, pgd, pud, pmd, pte, page_addr, page_offset);
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
 * Update struct thread_segment_info by information inside struct task_struct
*/
void update_info(struct task_struct *thread, struct thread_segment_info *info)
{
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
}

/**
 * Print all vma_struct within certain thread's mm->mmap
*/
void print_all_vma(pid_t pid, pid_t tgid, struct mm_struct *mm)
{
    struct vm_area_struct *mmap = mm->mmap;
    struct vm_area_struct *head;
    int idx = 0;

    printk("Print all vmas in [%ld, %ld]:\n", tgid, pid);
    for (head = mmap; head->vm_next; head = head->vm_next) {
        printk("vma #%d: [%ld ~ %ld]\n", idx++, head->vm_start, head->vm_end);
    }
}

/**
 * Print segments withen certain thread's mm (code, data, stack segment)
*/
void print_mm_segments(pid_t pid, pid_t tgid, struct mm_struct *mm)
{
    // Try to printk some information in dmesg
    printk("Print all mm segments in [%ld, %ld]:\n", tgid, pid);
    printk("code segment: [%ld ~ %ld]\n", mm->start_code, mm->end_code);
    printk("data segment: [%ld ~ %ld]\n", mm->start_data, mm->end_data);
    printk("stack segment: [%ld ~]\n", mm->start_stack);
}
