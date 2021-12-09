
#qemu-system-x86_64 -m 1024M -cpu qemu64 ProjectGuardian.img \
#  -drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF_CODE.fd,readonly=on \
#  -drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS.fd,readonly=on \
#  -net none
qemu-system-x86_64 -m 1024M -cpu qemu64 ProjectGuardian.img \
  -drive if=pflash,format=raw,unit=0,file=efi/OVMF_CODE.fd,readonly=on \
  -drive if=pflash,format=raw,unit=1,file=efi/OVMF_VARS.fd \
  -net none
