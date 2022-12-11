#! /bin/bash

KERNEL_VERSION=5.15.6

cd src
    cd linux-${KERNEL_VERSION}
        make defconfig
        make -j$(nproc) || exit
    cd ..
cd ..

cp src/linux-${KERNEL_VERSION}/arch/x86_64/boot/bzImage ./
