#! /bin/bash
# Update syscalls and Compile Kernel

source_dir=initrd/document/proj1
kernel_dir=src/linux-5.15.6

# sys_hello source code
cp ${source_dir}/hello/*.c ${source_dir}/hello/Makefile ${kernel_dir}/hello

# kernel Makefile, syscall header, syscall table
cp ${source_dir}/syscalls/Makefile ${kernel_dir}
cp ${source_dir}/syscalls/syscalls.h ${kernel_dir}/include/linux
cp ${source_dir}/syscalls/syscall_64.tbl ${kernel_dir}/arch/x86/entry/syscalls
