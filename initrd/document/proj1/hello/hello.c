#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <asm/pgtable.h>

void print_task_info(void);
void print_task_mm(void);
unsigned long virtual_to_physical(unsigned long);

SYSCALL_DEFINE1(hello, unsigned long, vir_addr)
{
    unsigned long phy_addr;

    print_task_info();
    print_task_mm();

    phy_addr = virtual_to_physical(vir_addr);
    printk("Virtual Address: [%lx]\nPhysical Address: [%lx]\n\n", vir_addr, phy_addr);

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
    printk("PageTable Entry: [%lx]\n", pte);
    page_addr = pte_val(pte) & PAGE_MASK;
    printk("Page Address: [%lx]\n", page_addr);
    page_offset = vir_addr & ~PAGE_MASK;
    printk("Page Index: [%lx]\n", page_offset);
    printk("Phy: [%lx]\n", page_addr + page_offset);
    phy_addr = page_addr | page_offset;

    return phy_addr;
}