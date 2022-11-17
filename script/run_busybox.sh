#! /bin/bash

BZIMAGE=bzImage
INITRD=initrd.img

qemu-system-x86_64 -kernel ${BZIMAGE} -initrd ${INITRD} -nographic -append 'console=ttyS0'
