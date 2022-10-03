#include <efi.h>
#include <efilib.h>
#include <string.h>

EFI_FILE_PROTOCOL* OpenFile(EFI_FILE_PROTOCOL* Volume, CHAR16* Path, EFI_HANDLE ImageHandle)
{
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
		Print(L"Framebuffer address %lx\n\tsize %d\n\twidth %d\n\theight %d\n\tpixelsperline %d\n",
			gop->Mode->FrameBufferBase,
			gop->Mode->FrameBufferSize,
			gop->Mode->Info->HorizontalResolution,
			gop->Mode->Info->VerticalResolution,
			gop->Mode->Info->PixelsPerScanLine
		);

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

	Print(L"%d, %d, %d\n", memcmp("test", "TEST", 4), memcmp("TEST", "test", 4), memcmp("TEST", "TEST", 4));

	return EFI_SUCCESS;
}
