LIBDIR = /usr/lib
SRCDIR = src
BINDIR = bin
EFIDIR = efi
BOOTDIR = $(SRCDIR)/boot
KERNDIR = $(SRCDIR)/kernel
STDLIBDIR = $(SRCDIR)/stdlib

LOADERSCRIPT = $(LIBDIR)/elf_x86_64_efi.lds
STARTUP = $(LIBDIR)/crt0-efi-x86_64.o

CC = gcc
LD = ld

CFLAGS = -I/usr/include/efi -I/usr/include/efi/x86_64 -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args

buildboot:
	@ echo Building booter...
	$(CC) $(CFLAGS) -c $(BOOTDIR)/mmboot64.c -o $(BOOTDIR)/mmboot64.o
	$(CC) $(CFLAGS) -c $(BOOTDIR)/colors.c -o $(BOOTDIR)/colors.o
	ld -shared -Bsymbolic -L$(LIBDIR) -T$(LOADERSCRIPT) $(STARTUP) $(BOOTDIR)/colors.o $(BOOTDIR)/mmboot64.o -lgnuefi -lefi -o $(BINDIR)/mmboot64.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 $(BINDIR)/mmboot64.so $(BINDIR)/mmboot64.efi
	rm $(BOOTDIR)/mmboot64.o $(BINDIR)/mmboot64.so
	rm $(BOOTDIR)/colors.o

buildkernel:
	@ echo Building kernel...
	touch $(BINDIR)/mmurtl64.elf

buildfloppy: buildboot buildkernel
	@ echo Building disk image...
	dd if=/dev/zero of=ProjectGuardian.img bs=512 count=2880
	mformat -i ProjectGuardian.img -f 1440 ::
	mmd -i ProjectGuardian.img ::/EFI
	mmd -i ProjectGuardian.img ::/EFI/BOOT
	mcopy -i ProjectGuardian.img $(BINDIR)/mmboot64.efi ::/EFI/BOOT
	mcopy -i ProjectGuardian.img $(EFIDIR)/startup.nsh ::
	mcopy -i ProjectGuardian.img $(BINDIR)/mmurtl64.elf ::

vmdk: buildfloppy
	@ echo Building vmdk image...
	rm -rf ProjectGuardian.vmdk
	vboxmanage internalcommands createrawvmdk -filename ProjectGuardian.vmdk -rawdisk ProjectGuardian.img

clean:
	@ echo Cleaning up...
	rm -rf bin/*
	rm -rf ProjectGuardian.img

test: buildfloppy
	qemu-system-x86_64 -machine q35 -m 1024M -cpu qemu64 \
		-drive format=raw,file=ProjectGuardian.img \
		-drive if=pflash,format=raw,unit=0,file=efi/OVMF_CODE.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=efi/OVMF_VARS.fd \
		-net none
