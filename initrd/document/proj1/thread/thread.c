#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <link.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define SYSCALL_HELLO 449
#define LIB_ADDR_BUF_SIZE 64

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

static void print_flags(const ElfW(Phdr) *p) {
    putchar((p->p_flags & PF_R) ? 'r' : '-');
    putchar((p->p_flags & PF_W) ? 'w' : '-');
    putchar((p->p_flags & PF_X) ? 'x' : '-');

    ElfW(Word) other_flags = p->p_flags & ~(PF_R | PF_W | PF_X);
    if (other_flags) {
        printf("/%08x", other_flags);
    }
}

static int callback(struct dl_phdr_info *info, size_t size, void *data) {
	const char* seg_name = (info->dlpi_name[0] == '\0' ? "[main]" : info->dlpi_name);
    struct seg_info* info_buf = (struct seg_info*) data;

    for (int i = 0; i < info->dlpi_phnum; i++) {
        if (info->dlpi_phdr[i].p_type != PT_LOAD) {
            continue;
        }
        unsigned long start = (unsigned long)(info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);
        unsigned long end = start + info->dlpi_phdr[i].p_memsz;

        strcpy(info_buf->library_name[info_buf->library_num], seg_name);
        info_buf->library_addrs[info_buf->library_num][0] = start;
        info_buf->library_addrs[info_buf->library_num][1] = end;
        info_buf->library_num++;

        printf("%016lx-%016lx ", start, end);
        print_flags(&info->dlpi_phdr[i]);
		printf(" %08lx   %s", end - start, seg_name);
        putchar('\n');
    }
	putchar('\n');
    return 0;
}

void* child1(void*);
void* child2(void*);
void show_info(struct seg_info *);

int main()
{
    struct seg_info* info = (struct seg_info*) malloc(sizeof(struct seg_info));
    memset(info->library_addrs, 0, 2 * LIB_ADDR_BUF_SIZE * sizeof(unsigned long));
    info->library_num = 0;

    dl_iterate_phdr(callback, info);

    long int ret = syscall(SYSCALL_HELLO, info);
    show_info(info);

    free(info);
    return 0;
}

void show_info(struct seg_info *info)
{
    printf("Task [%d, %d]:\n", info->pid, info->tgid);
    printf("total vm: %ld\n", info->total_vm);
    printf("data segment: [%016lx ~ %016lx]\n", info->start_data, info->end_data);
    printf("code segment: [%016lx ~ %016lx]\n", info->start_code, info->end_code);
    printf("brk segment: [%016lx ~ %016lx]\n", info->start_brk, info->brk);
    printf("stack segment: [%016lx ~]\n", info->start_stack);
    printf("libraries:\n");
    for (int i = 0; i < info->library_num; i++) {
        unsigned long *addr = info->library_addrs[i];
        char* name = info->library_name[i];
        printf("- [%016lx - %016lx]     %s\n", addr[0], addr[1], name);
    }
    printf("=========\n\n");
}
