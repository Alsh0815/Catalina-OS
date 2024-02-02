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
#include "elf.hpp"
#include "memory.h"

void CalcLoadAddressRange(Elf64_Ehdr *ehdr, UINT64 *first, UINT64 *last)
{
    Elf64_Phdr *phdr = (Elf64_Phdr *)((UINT64)ehdr + ehdr->e_phoff);
    *first = MAX_UINT64;
    *last = 0;
    for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i)
    {
        if (phdr[i].p_type != PT_LOAD)
            continue;
        *first = MIN(*first, phdr[i].p_vaddr);
        *last = MAX(*last, phdr[i].p_vaddr + phdr[i].p_memsz);
    }
}

const CHAR16 *GetPixelFormatUnicode(EFI_GRAPHICS_PIXEL_FORMAT fmt)
{
    switch (fmt)
    {
    case PixelRedGreenBlueReserved8BitPerColor:
        return L"PixelRedGreenBlueReserved8BitPerColor";
    case PixelBlueGreenRedReserved8BitPerColor:
        return L"PixelBlueGreenRedReserved8BitPerColor";
    case PixelBitMask:
        return L"PixelBitMask";
    case PixelBltOnly:
        return L"PixelBltOnly";
    case PixelFormatMax:
        return L"PixelFormatMax";
    default:
        return L"InvalidPixelFormat";
    }
}

void Halt(void)
{
    while (1)
        __asm__("hlt");
}

EFI_STATUS OpenGOP(EFI_HANDLE image_handle, EFI_GRAPHICS_OUTPUT_PROTOCOL **gop)
{
    EFI_STATUS status;
    UINTN num_gop_handles = 0;
    EFI_HANDLE *gop_handles = NULL;

    status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &num_gop_handles,
        &gop_handles);
    if (EFI_ERROR(status))
    {
        return status;
    }

    status = gBS->OpenProtocol(
        gop_handles[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)gop,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(status))
    {
        return status;
    }

    gBS->FreePool(gop_handles);

    return EFI_SUCCESS;
}

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

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    OpenGOP(ImageHandle, &gop);
    Print(
        L"Res: %u x %u, Format: %s, %u pixels/ln\n",
        gop->Mode->Info->HorizontalResolution,
        gop->Mode->Info->VerticalResolution,
        GetPixelFormatUnicode(gop->Mode->Info->PixelFormat),
        gop->Mode->Info->PixelsPerScanLine);
    Print(
        L"Frame Buffer: 0x%0lx - 0x%0lx, Size: %lu bytes\n",
        gop->Mode->FrameBufferBase,
        gop->Mode->FrameBufferBase + gop->Mode->FrameBufferSize,
        gop->Mode->FrameBufferSize);

    UINT8 *frame_buffer = (UINT8 *)gop->Mode->FrameBufferBase;
    for (UINTN i = 0; i < gop->Mode->FrameBufferSize; ++i)
    {
        frame_buffer[i] = 255;
    }

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

    EFI_STATUS status;
    EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;

    VOID *kernel_buffer;
    status = gBS->AllocatePool(EfiLoaderData, kernel_file_size, &kernel_buffer);
    if (EFI_ERROR(status))
    {
        Print(L"failed to allocate pool: %r\n", status);
        Halt();
    }
    status = kernel_file->Read(kernel_file, &kernel_file_size, kernel_buffer);
    if (EFI_ERROR(status))
    {
        Print(L"error: %r\n", status);
        Halt();
    }

    Print(L"Kernel: 0x%0lx (%lu bytes)\n", kernel_base_addr, kernel_file_size);

    Elf64_Ehdr *kernel_ehdr = (Elf64_Ehdr *)kernel_buffer;
    UINT64 kernel_first_addr, kernel_last_addr;
    CalcLoadAddressRange(kernel_ehdr, &kernel_first_addr, &kernel_last_addr);

    UINTN num_pages = (kernel_last_addr - kernel_first_addr + 0xfff) / 0x1000;
    status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, num_pages, &kernel_first_addr);
    if (EFI_ERROR(status))
    {
        Print(L"failed to allocate pages: %r\n", status);
        Halt();
    }

    status = gBS->ExitBootServices(ImageHandle, memmap.map_key);
    if (EFI_ERROR(status))
    {
        status = GetMemoryMap(&memmap);
        if (EFI_ERROR(status))
        {
            Print(L"failed to get memory map: %r\n", status);
            Halt();
        }
        status = gBS->ExitBootServices(ImageHandle, memmap.map_key);
        if (EFI_ERROR(status))
        {
            Print(L"Could not exit boot service: %r\n", status);
            Halt();
        }
    }

    UINT64 entry_addr = *(UINT64 *)(kernel_base_addr + 24);

    typedef void EntryPointType(void);
    EntryPointType *entry_point = (EntryPointType *)entry_addr;
    entry_point();

    return EFI_SUCCESS;
}