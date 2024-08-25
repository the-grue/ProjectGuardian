#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include <string.h>
#include <mmkernel.h>

EFI_MEMORY_DESCRIPTOR* Map = NULL;
UINTN MapSize, MapKey;
UINTN MemSize = 0;
UINTN DescriptorSize;
UINT32 DescriptorVersion;

mmKernelTable ktable;

EFI_FILE_PROTOCOL* OpenFile(EFI_FILE_PROTOCOL* Volume, CHAR16* Path, EFI_HANDLE ImageHandle)
{
	ktable.yarray = NULL;
	EFI_FILE_PROTOCOL* LoadFile;
	EFI_LOADED_IMAGE_PROTOCOL* LoadImage;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
	EFI_STATUS Status = EFI_SUCCESS;

	uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle, &gEfiLoadedImageProtocolGuid, (void**) &LoadImage);

	uefi_call_wrapper(BS->HandleProtocol, 3, LoadImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**) &FileSystem);

	if(Volume == NULL)
		uefi_call_wrapper(FileSystem->OpenVolume, 2, FileSystem, &Volume);

	Status = uefi_call_wrapper(Volume->Open, 5, Volume, &LoadFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

	if(Status != EFI_SUCCESS)
		return NULL;

	return LoadFile;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	extern uint32_t colors[];

	InitializeLib(ImageHandle, SystemTable);

	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
	EFI_STATUS status;

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
	if(EFI_ERROR(status))
	{
		Print(L"Unable to locate GOP\n");
		return status;
	}
	else
		Print(L"Found GOP\n");

	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
	UINTN SizeOfInfo, numModes, nativeMode;

	status = uefi_call_wrapper(gop->QueryMode, 4, gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &info);

	if(status == EFI_NOT_STARTED)
		status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
	if(EFI_ERROR(status))
	{
		Print(L"Unable to get native mode");
		return status;
	}
	else
	{
		nativeMode = gop->Mode->Mode;
		numModes = gop->Mode->MaxMode;
		Print(L"Native mode: %ld\n", nativeMode);
	}

	UINTN mode = nativeMode;

	for(UINTN i = 0; i < numModes; i++)
	{
		status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &SizeOfInfo, &info);
		Print(L"mode %3d width %4d height %4d ",
			i,
			info->HorizontalResolution,
			info->VerticalResolution
		);

		if(i & 1)
			Print(L"\n");
		else
			Print(L"  ");

		if(info->HorizontalResolution == 1280 &&
		   info->VerticalResolution == 720)
			mode = i;
	}

	Print(L"\n");

	CHAR16 inmode[4];
	UINTN mymode;

	Input(L"Enter desired mode number: ", (CHAR16 *) &inmode, 4);

	mymode = Atoi((const CHAR16 *) &inmode);

	PrintAt(0, 0, L"\n");

	if(mymode <0 || mymode > numModes)
		mode = nativeMode;
	else
		mode = mymode;

	status = uefi_call_wrapper(gop->SetMode, 2, gop, mode);

	if(EFI_ERROR(status))
	{
		Print(L"Unable to set mode %03d\n", mode);
		return status;
	}
	else
	{
		ktable.fb.FBBase = (void*) gop->Mode->FrameBufferBase;
		ktable.fb.FBSize = gop->Mode->FrameBufferSize;
		ktable.fb.Width = gop->Mode->Info->HorizontalResolution;
		ktable.fb.Height = gop->Mode->Info->VerticalResolution;
		ktable.fb.PPScanLine = gop->Mode->Info->PixelsPerScanLine;
		ktable.fb.BPP = 4;

		Print(L"Framebuffer address %lx\n\tsize %d\n\twidth %d\n\theight %d\n\tpixelsperline %d\n",
			ktable.fb.FBBase,
			ktable.fb.FBSize,
			ktable.fb.Width,
			ktable.fb.Height,
			ktable.fb.PPScanLine
		);
	}

	Print(L"Size of UINTN is %d\n", sizeof(UINTN));

	for(UINTN x = 0; x < 1024; x+=4)
		for(UINTN z = 0; z < 4; z++)
			for(UINTN y = 0; y < 100; y++)
				*((uint32_t*)(gop->Mode->FrameBufferBase + (4 * gop->Mode->Info->PixelsPerScanLine) + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * (x+z))) = colors[x/4];

	Print(L"Booting MMURTL-64\n");

	EFI_FILE_PROTOCOL* MMKernel = OpenFile(NULL, L"mmurtl64.elf", ImageHandle);

	if(MMKernel == NULL)
		Print(L"Failed to load MMKernel\n");
	else
		Print(L"MMKernel loaded...\n");

	Elf64_Ehdr header;
	UINTN FileInfoSize;
	EFI_FILE_INFO* FileInfo;

	uefi_call_wrapper(MMKernel->GetInfo, 4, MMKernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, FileInfoSize, (void**)&FileInfo);
	uefi_call_wrapper(MMKernel->GetInfo, 4, MMKernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

	UINTN size = sizeof(header);
	uefi_call_wrapper(MMKernel->Read, 3, MMKernel, &size, &header);

	if(
                memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
                header.e_ident[EI_CLASS] != ELFCLASS64 ||
                header.e_ident[EI_DATA] != ELFDATA2LSB ||
                header.e_type != ET_EXEC ||
                header.e_machine != EM_X86_64 ||
                header.e_version != EV_CURRENT
        )
	{
		Print(L"Bad ELF format!\n");
		return -1;
	}
	else
		Print(L"ELF-64 format kernel!\n");

	Elf64_Phdr* pheaders;

	uefi_call_wrapper(MMKernel->SetPosition, 2, MMKernel, header.e_phoff);
	size = header.e_phnum * header.e_phentsize;
	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, size, (void**)&pheaders);
	uefi_call_wrapper(MMKernel->Read, 3, MMKernel, &size, pheaders);

	for(Elf64_Phdr* pheader = pheaders;
	(char*)pheader < (char*)pheaders + header.e_phnum * header.e_phentsize;
	pheader = (Elf64_Phdr*)((char*)pheader + header.e_phentsize)
	)
	{
		switch(pheader->p_type)
		{
			case PT_LOAD:
			{
				int pages = (pheader->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = pheader->p_paddr;
				uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, pages, &segment);

				uefi_call_wrapper(MMKernel->SetPosition, 2, MMKernel, pheader->p_offset);
				size = pheader->p_filesz;
				uefi_call_wrapper(MMKernel->Read, 3, MMKernel, &size, (void*)segment);
			break;
			}
		}
	}

	/* Get initial memory size and map  */

	uefi_call_wrapper(BS->GetMemoryMap, 5, &MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, MapSize, (void**)&Map);
	uefi_call_wrapper(BS->GetMemoryMap, 5, &MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

	/* Calculate MemSize */

	UINTN MapEntries = MapSize / DescriptorSize;

	for(int x = 0; x < MapEntries; x++)
	{
		EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR *)((UINTN)Map + (x * DescriptorSize));

		if(descriptor->Type != EfiReservedMemoryType)
			MemSize = MemSize + (descriptor->NumberOfPages);
	}

	/* Free previous map, then get space for Page Allocation Map (PAM)  */

	uefi_call_wrapper(BS->FreePool, 1, (void *)&Map);
	Map = NULL;

	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, MemSize / 4096 / 8, (void**)&ktable.PAM);

	Print(L"PAM address %llx\n", ktable.PAM);

	/* Now, get final memory size and map to pass to kernel  */

	uefi_call_wrapper(BS->GetMemoryMap, 5, &MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, MapSize, (void**)&Map);
	uefi_call_wrapper(BS->GetMemoryMap, 5, &MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

	ktable.mmap.mMap = Map;
	ktable.mmap.MapSize = MapSize;
	ktable.mmap.DescriptorSize = DescriptorSize;

	Print(L"Starting MMURTL-64...\n");

	uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, MapKey);

	int(*KernelStart)(mmKernelTable *) = ((__attribute__((sysv_abi)) int (*)(mmKernelTable *) ) header.e_entry);

	Print(L"%X rules!\n", KernelStart(&ktable));

	return EFI_SUCCESS;
}
