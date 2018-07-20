# Kernel
a kernel i'm making i guess, just called "Kernel" for now

requirements for compilation:
i686-elf C compiler and assembler, along with NASM for boot.s (use -felf32 switch)

compilation steps:
1: compile the following files to the following:
syntax: (file to be compiled) -> (compiled file)

boot.s -> boot.o (compile with NASM and -felf32 switch)
kernel.c -> kernel.o
rw.c -> rw.o
var.c -> var.o
print.c -> print.o
dtb.c -> dtb.o
dtb.s -> dtba.o
pic.c -> pic.o
paging.c -> paging.o
malloc/libmalloc.c -> malloc.o (under BSD 3-clause license, don't compile with -Wall or -Wextra)
malloc/libmalloc_hooks.c -> malloch.o

2: link all of them, linker file is linker.ld
put boot.o and kernel.o as the first files to link always

3: run it with the --kernel switch in qemu


