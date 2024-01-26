[Defines]
    PLATFORM_NAME                  = CatalinaLoaderPkg
    PLATFORM_GUID                  = 65e09a66-e001-41c6-e149-e949207c28b0
    PLATFORM_VERSION               = 0.1
    DSC_SPECIFICATION              = 0x00010005
    OUTPUT_DIRECTORY               = Build/CatalinaLoader$(ARCH)
    SUPPORTED_ARCHITECTURES        = X64
    BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT

[LibraryClasses]
    UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
    UefiLib|MdePkg/Library/UefiLib/UefiLib.inf

    BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
    BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
    DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
    DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
    MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
    PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
    PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
    RegisterFilterLib|MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
    UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
    UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf

[Components]
    CatalinaLoaderPkg/Loader.inf