cmd_/home/steven/Projects/dfu/linux/cp210x.ko := ld -r -m elf_x86_64 -z max-page-size=0x200000 -T ./scripts/module-common.lds --build-id  -o /home/steven/Projects/dfu/linux/cp210x.ko /home/steven/Projects/dfu/linux/cp210x.o /home/steven/Projects/dfu/linux/cp210x.mod.o ;  true