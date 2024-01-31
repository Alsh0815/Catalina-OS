#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DiskIo2.h>
#include <Protocol/BlockIo.h>
#include <Uefi.h>

#include "Build.h"
#include "memory.h"

EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root)
{
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

    gBS->OpenProtocol(
        image_handle,
        &gEfiLoadedImageProtocolGuid,
        (VOID **)&loaded_image,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    gBS->OpenProtocol(
        loaded_image->DeviceHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID **)&fs,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    fs->OpenVolume(fs, root);

    return EFI_SUCCESS;
}

EFI_STATUS UefiMain(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable)
{
    Print(L"[Catalina-OS]\n");
    Print(L"Version : %d.%d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, REVIS_VERSION);
    Print(L"Hello, Catalina-OS World!\n");

    CHAR8 memmap_buf[4096 * 4];
    struct MemoryMap memmap = {sizeof(memmap_buf), memmap_buf, 0, 0, 0, 0};
    GetMemoryMap(&memmap);

    EFI_FILE_PROTOCOL *root_dir;
    OpenRootDir(ImageHandle, &root_dir);

    EFI_FILE_PROTOCOL *memmap_file;
    root_dir->Open(root_dir, &memmap_file, L"\\memmap", EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);

    SaveMemoryMap(&memmap, memmap_file);
    memmap_file->Close(memmap_file);

    return EFI_SUCCESS;
}