#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <asm/pgtable.h>

#define LIB_ADDR_BUF_SIZE 64

void print_task_info(void);
void print_task_mm(void);
unsigned long virtual_to_physical(unsigned long);

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

    char library_name[LIB_ADDR_BUF_SIZE][256];
    unsigned long library_addrs[LIB_ADDR_BUF_SIZE][2];
    int library_num;
};

SYSCALL_DEFINE1(hello, struct seg_info __user *, info)
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
    info_buf->start_code = virtual_to_physical(current->mm->start_code);
    info_buf->end_code = virtual_to_physical(current->mm->end_code);
    info_buf->start_data = virtual_to_physical(current->mm->start_data);
    info_buf->end_data = virtual_to_physical(current->mm->end_data);
    info_buf->start_brk = virtual_to_physical(current->mm->start_brk);
    info_buf->brk = virtual_to_physical(current->mm->brk);
    info_buf->start_stack = virtual_to_physical(current->mm->start_stack);

    for (int i = 0; i < info_buf->library_num; i++) {
        unsigned long *addr = info_buf->library_addrs[i];
        info_buf->library_addrs[i][0] = virtual_to_physical(addr[0]);
        info_buf->library_addrs[i][1] = virtual_to_physical(addr[1]);
    }

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

unsigned long virtual_to_physical(unsigned long vir_addr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *ptep, pte;
    struct mm_struct *mm = current->mm;
    unsigned long page_addr = 0, page_offset = 0, phy_addr = 0;

    pgd = pgd_offset(mm, vir_addr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        return 0;
    }

    p4d = p4d_offset(pgd, vir_addr);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
        return 0;
    }

    pud = pud_offset(p4d, vir_addr);
    if (pud_none(*pud) || pud_bad(*pud)) {
        return 0;
    }

    pmd = pmd_offset(pud, vir_addr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        return 0;
    }

    ptep = pte_offset_kernel(pmd, vir_addr);
    if (pte_none(*ptep)) {
        return 0;
    }
    pte = *ptep;
    page_addr = pte_val(pte) & PAGE_MASK;
    page_offset = vir_addr & ~PAGE_MASK;
    phy_addr = page_addr | page_offset;

    return phy_addr;
}
