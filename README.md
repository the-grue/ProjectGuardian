# ProjectGuardian

Required filesets for Ubuntu 20.04/22.04:

  sudo apt-get install build-essential git gnu-efi ovmf qemu-system-x86 mtools

Pull down the repo and then copy the OVMF files from /usr/share/OVMF to efi:

  cd efi

  cp -p /usr/share/OVMF/OVMF_????.fd .

For Ubuntu 24.04, you'll need to copy files and create symbolic links as follows:

  cp -p /usr/share/OVMF/OVMF_????_4M.fd .

  ln -s OVMF_CODE_4M.fd OVMF_CODE.fd

  ln -s OVMF_VARS_4M.fd OVMF_VARS.fd

This will build the code and launch qemu:

  make test

If you are using VirtualBox, you can create a vmdk disk image that points to the .img
file using:

  make vmdk

You can then create a VirtualBox VM and attach the vmdk file.  Keep in mind that you
should just do "make buildfloppy" after this.  If you run make vmdk again, it will
assign a new UUID and you'll need to redo your VM.
