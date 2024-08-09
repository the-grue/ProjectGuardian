LIBDIR = /usr/lib
SRCDIR = src
BINDIR = bin
EFIDIR = efi
LOGDIR = log
BOOTDIR = $(SRCDIR)/boot
KERNDIR = $(SRCDIR)/kernel
STDLIBDIR = $(SRCDIR)/stdlib/src
GRLIBDIR = $(SRCDIR)/graphics/src

LOADERSCRIPT = $(LIBDIR)/elf_x86_64_efi.lds
STARTUP = $(LIBDIR)/crt0-efi-x86_64.o
KLDSCRIPT = $(KERNDIR)/mmkernel.ld

CC = gcc
LD = ld
AR = ar

CFLAGS = -I/usr/include/efi -I/usr/include/efi/x86_64 -Isrc/stdlib/include -Isrc/graphics/include -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args
ARFLAGS = rcs
KCFLAGS = -Isrc/stdlib/include -Isrc/graphics/include -I/usr/include/efi -I/usr/include/efi/x86_64 -ffreestanding -fshort-wchar
KLDFLAGS = -T $(KLDSCRIPT) -static -Bsymbolic -nostdlib -Llib

buildstdlib:
	@ echo Building stdlib...
	$(CC) $(CFLAGS) -c $(STDLIBDIR)/memcmp.c -o $(STDLIBDIR)/memcmp.o
	$(CC) $(CFLAGS) -c $(STDLIBDIR)/abs.c -o $(STDLIBDIR)/abs.o
	$(AR) $(ARFLAGS) lib/libstd.a $(STDLIBDIR)/memcmp.o $(STDLIBDIR)/abs.o
	rm $(STDLIBDIR)/memcmp.o $(STDLIBDIR)/abs.o

buildgraphlib:
	@ echo Building graphics library...
	$(CC) $(CFLAGS) -c $(GRLIBDIR)/putpixel.c -o $(GRLIBDIR)/putpixel.o
	$(CC) $(CFLAGS) -c $(GRLIBDIR)/outtext.c -o $(GRLIBDIR)/outtext.o
	$(CC) $(CFLAGS) -c $(GRLIBDIR)/line.c -o $(GRLIBDIR)/line.o
	$(CC) $(CFLAGS) -c $(GRLIBDIR)/rectangle.c -o $(GRLIBDIR)/rectangle.o
	$(AR) $(ARFLAGS) lib/libGGI.a $(GRLIBDIR)/putpixel.o $(GRLIBDIR)/outtext.o
	$(AR) $(ARFLAGS) lib/libGGI.a $(GRLIBDIR)/line.o $(GRLIBDIR)/rectangle.o
	rm $(GRLIBDIR)/putpixel.o $(GRLIBDIR)/outtext.o $(GRLIBDIR)/line.o
	rm $(GRLIBDIR)/rectangle.o

buildboot:
	@ echo Building booter...
	$(CC) $(CFLAGS) -c $(BOOTDIR)/mmboot64.c -o $(BOOTDIR)/mmboot64.o
	$(CC) $(CFLAGS) -c $(BOOTDIR)/colors.c -o $(BOOTDIR)/colors.o
	ld -M -shared -Bsymbolic -L$(LIBDIR) -Llib -T$(LOADERSCRIPT) $(STARTUP) $(BOOTDIR)/colors.o $(BOOTDIR)/mmboot64.o -lstd -lgnuefi -lefi -o $(BINDIR)/mmboot64.so > $(LOGDIR)/linkmap.out
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 $(BINDIR)/mmboot64.so $(BINDIR)/mmboot64.efi
	rm $(BOOTDIR)/mmboot64.o $(BINDIR)/mmboot64.so
	rm $(BOOTDIR)/colors.o

buildkernel:
	@ echo Building kernel...
	$(CC) $(KCFLAGS) -c $(KERNDIR)/mmkernel.c -o $(KERNDIR)/mmkernel.o
	$(CC) $(KCFLAGS) -c $(BOOTDIR)/colors.c -o $(KERNDIR)/colors.o
	$(LD) $(KLDFLAGS) -o $(BINDIR)/mmurtl64.elf $(KERNDIR)/mmkernel.o $(KERNDIR)/colors.o -lGGI -lstd
	rm $(KERNDIR)/mmkernel.o $(KERNDIR)/colors.o

buildfloppy: buildstdlib buildgraphlib buildboot buildkernel
	@ echo Building disk image...
	dd if=/dev/zero of=ProjectGuardian.img bs=512 count=2880
	mformat -i ProjectGuardian.img -f 1440 ::
	sfdisk --part-type ProjectGuardian.img 1 ef
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
	rm -rf log/*
	rm -rf lib/*
	rm -rf ProjectGuardian.img

test: buildfloppy
	qemu-system-x86_64 -machine q35 -m 1024M -cpu qemu64 \
		-drive format=raw,file=ProjectGuardian.img \
		-drive if=pflash,format=raw,unit=0,file=efi/OVMF_CODE.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=efi/OVMF_VARS.fd \
		-net none
