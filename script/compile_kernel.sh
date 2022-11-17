#! /bin/bash

KERNEL_VERSION=5.15.6

cd src
    cd linux-${KERNEL_VERSION}
        make -j$(nproc) || exit
    cd ..
cd ..

cp src/linux-${KERNEL_VERSION}/arch/x86_64/boot/bzImage ./
cp src/linux-5.15.6/arch/x86_64/boot/bzImage ./
