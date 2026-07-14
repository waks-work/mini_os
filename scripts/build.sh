#!/usr/bin/env bash
set -e

mkdir -p build

## COMPILE BOOTLOADER
clang -c boot/boot.s -o build/boot.o

#verify it :
file build/boot.o
readelf -h build/boot.o
objdump -d build/boot.o

# compile waks_cstd
../waks_cstd/scripts/build.sh
build/libwaks_cstd.a

# compile kernel
clang \
    -ffreestanding \
    -fno-stack-protector \
    -fno-pic \
    -m64 \
    -nostdlib \
    -Wall \
    -Wextra \
    -std=c11 \
    -I ../waks_cstd/include \
    -c kernel/main.c \
    -o build/kernel.o


## LINK
# boot.o
# kernel.o
# libwaks_cstd.a
#      ld
#    kernel.elf

ld \
    -m elf_x86_64 \
    -T kernel/linker.ld \
    build/boot.o \
    build/kernel.o \
    ../waks_cstd/build/libwaks_cstd.a \
    -o build/kernel.elf# kernel.bin

file build/kernel.elf
readelf -h build/kernel.elf
readelf -S build/kernel.elf
nm build/kernel.elf

## CONVERT IT TO BINARY
objcopy \
    -O binary \
    build/kernel.elf \
    build/kernel.bin

## QEMU SETUP
qemu-system-x86_64 \
    -kernel build/kernel.bin
