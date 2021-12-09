@echo -off
mode 80 25

cls
if exist fs0:\efi\boot\mmboot64.efi then
 fs0:
 efi\boot\mmboot64.efi
 goto END
endif

echo "No bootloader installed"

:END
