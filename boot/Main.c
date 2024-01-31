#include <Guid/FileInfo.h>
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

    EFI_FILE_PROTOCOL *kernel_file;
    root_dir->Open(
        root_dir, &kernel_file, L"\\kernel.elf",
        EFI_FILE_MODE_READ, 0);

    UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
    UINT8 file_info_buffer[file_info_size];
    kernel_file->GetInfo(
        kernel_file, &gEfiFileInfoGuid,
        &file_info_size, file_info_buffer);

    EFI_FILE_INFO *file_info = (EFI_FILE_INFO *)file_info_buffer;
    UINTN kernel_file_size = file_info->FileSize;

    EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;
    gBS->AllocatePages(
        AllocateAddress, EfiLoaderData,
        (kernel_file_size + 0xfff) / 0x1000, &kernel_base_addr);
    kernel_file->Read(kernel_file, &kernel_file_size, (VOID *)kernel_base_addr);
    Print(L"Kernel: 0x%0lx (%lu bytes)\n", kernel_base_addr, kernel_file_size);

    EFI_STATUS status;

    status = gBS->ExitBootServices(ImageHandle, memmap.map_key);
    if (EFI_ERROR(status))
    {
        status = GetMemoryMap(&memmap);
        if (EFI_ERROR(status))
        {
            Print(L"failed to get memory map: %r\n", status);
            while (1)
                ;
        }
        status = gBS->ExitBootServices(ImageHandle, memmap.map_key);
        if (EFI_ERROR(status))
        {
            Print(L"Could not exit boot service: %r\n", status);
            while (1)
                ;
        }
    }

    UINT64 entry_addr = *(UINT64 *)(kernel_base_addr + 24);

    typedef void EntryPointType(void);
    EntryPointType *entry_point = (EntryPointType *)entry_addr;
    entry_point();

    return EFI_SUCCESS;
}