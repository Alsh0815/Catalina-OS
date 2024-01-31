#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Uefi.h>

#ifndef _H_BOOT_MEMORY_

#define _H_BOOT_MEMORY_

struct MemoryMap
{
    UINTN buffer_size;
    VOID *buffer;
    UINTN map_size;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
};

const CHAR16 *GetMemoryTypeUnicode(EFI_MEMORY_TYPE type);
EFI_STATUS GetMemoryMap(struct MemoryMap *map);
EFI_STATUS SaveMemoryMap(struct MemoryMap *map, EFI_FILE_PROTOCOL *file);

#endif