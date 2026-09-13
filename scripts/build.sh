#!/usr/bin/env bash
set -e
mkdir -p build

## COMMON C FLAGS
CFLAGS=(
    -target x86_64-unknown-none-elf
    -ffreestanding
    -fno-stack-protector
    -fno-pic
    -nostdlib
    -DWAKS_TARGET_BARE_METAL
    -Wno-pointer-sign
    -Wall
    -Wextra
    -std=c11
    -Ithirdparty/waks_cstd
    -Ikernel
)

# Compile the bootloader
clang -c boot/boot.s -o build/boot.o

# Compile all files in kernel 
KERNEL_OBJ=()
for c_file in kernel/*.c; do 
    filename=$(basename "$c_file" .c)
    obj_file="build/${filename}.o"
    clang "${CFLAGS[@]}" -c "$c_file" -o "$obj_file"
    KERNEL_OBJ+=("$obj_file")
done

## 3. LINK (64-bit throughout — boot.s's long-mode trampoline makes this consistent again)
clang -target x86_64-unknown-none-elf -ffreestanding -nostdlib -static \
    -T kernel/linker.ld \
    build/boot.o \
    "${KERNEL_OBJ[@]}" \
    thirdparty/waks_cstd/libwaks.a \
    -o build/kernel.elf

## 4. VERIFY MULTIBOOT COMPLIANCE
if command -v grub-file >/dev/null 2>&1; then
    if grub-file --is-x86-multiboot build/kernel.elf; then
        echo "[+] Multiboot header confirmed."
    else
        echo "[-] Error: File is not multiboot compliant."
        exit 1
    fi
fi

## 5. BUILD A BOOTABLE ISO VIA GRUB
# QEMU's built-in -kernel Multiboot loader only accepts 32-bit ELFs.
# Since kernel.elf is genuinely 64-bit, GRUB has to do the loading instead —
# it doesn't share that restriction.
rm -rf build/isodir
mkdir -p build/isodir/boot/grub
cp build/kernel.elf build/isodir/boot/kernel.elf

cat > build/isodir/boot/grub/grub.cfg << 'EOF'
menuentry "mini_os" {
    multiboot /boot/kernel.elf
    boot
}
EOF

grub-mkrescue -o build/mini_os.iso build/isodir
echo "[+] Built build/mini_os.iso"

## 6. RUN VIA THE ISO (not -kernel, since that path is 32-bit-only)
qemu-system-x86_64 -cdrom build/mini_os.iso -nographic
