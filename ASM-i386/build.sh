#!/bin/sh

SRC=hello-elf-i386
PNAME=out

echo "assembling..."
nasm -f elf32 ${SRC}.s -o ${SRC}.o
echo "done!"
echo ""
echo "linking..."
ld -m elf_i386 -s ${SRC}.o -o ${PNAME}
echo "done!"

rm -f ${SRC}.o