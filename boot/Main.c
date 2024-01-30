#include <Library/UefiLib.h>
#include <Uefi.h>

EFI_STATUS UefiMain(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable)
{
    Print(L"Hello, Catalina-OS World!\n");
    return EFI_SUCCESS;
}