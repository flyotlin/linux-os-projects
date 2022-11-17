#! /usr/bin/python3
import argparse
import os
import subprocess


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-S", "--update-syscalls", action="store_true")
    parser.add_argument("-P", "--update-program", action="store_true")
    parser.add_argument("-K", "--compile-kernel", action="store_true")
    parser.add_argument("-B", "--compile-busybox", action="store_true")
    parser.add_argument("-I", "--prepare-initrd", action="store_true")
    parser.add_argument("-R", "--run-busybox", action="store_true")
    return parser.parse_args()

def main():
    args = parse_args()
    pwd = os.path.abspath(os.path.dirname(__file__))
    script_path = os.path.join(pwd, "script")

    if args.update_syscalls:
        path = os.path.join(script_path, "update_syscalls.sh")
        subprocess.call([path])
    # TODO: --FUTURE--
    # if kernel src exists: update & compile
    # else: download & untar & update & compile
    if args.compile_kernel:
        path = os.path.join(script_path, "compile_kernel.sh")
        subprocess.call([path])
    if args.compile_busybox:
        path = os.path.join(script_path, "compile_busybox.sh")
        subprocess.call([path])

    if args.update_program:
        path = os.path.join(script_path, "update_program.sh")
        subprocess.call([path])
    if args.prepare_initrd:
        path = os.path.join(script_path, "prepare_initrd.sh")
        subprocess.call([path])
    if args.run_busybox:
        path = os.path.join(script_path, "run_busybox.sh")
        subprocess.call([path])


if __name__ == "__main__":
    main()
