#! /bin/bash

BUSYBOX_VERSION=1.34.1
cd src
    cd busybox-${BUSYBOX_VERSION}
        make defconfig
        sed 's/^.*CONFIG_STATIC[^_].*$/CONFIG_STATIC=y/g' -i .config
        make -j$(nproc) busybox || exit
    cd ..
cd ..
