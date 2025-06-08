#include "fs.h"


EFI_GUID gEfiFileInfoGuid = EFI_FILE_INFO_ID;

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image) {
    // From OS dev wiki
    EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
    EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
    EFI_FILE_IO_INTERFACE *IOVolume;                        /* file system interface */
    EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
    EFI_FILE_HANDLE Volume;                                 /* the volume's interface */

    /* get the loaded image protocol interface for our "image" */
    uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
    /* get the volume handle */
    uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*)&IOVolume);
    uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);

    return Volume;
}

EFI_FILE_INFO* GetFileInfo(EFI_FILE_HANDLE FileHandle){
    EFI_STATUS status;
    UINTN FileInfoSize = 0;
    EFI_FILE_INFO *FileInfo = NULL;

    status = FileHandle->GetInfo(FileHandle, &gEfiFileInfoGuid, &FileInfoSize, NULL);
    if (status != EFI_BUFFER_TOO_SMALL) {
        Err(L"[ERROR] GetInfo (size query) failed.\r\n");
    }

    FileInfo = (EFI_FILE_INFO*) Allocate(FileInfoSize);
    if (FileInfo == NULL){
        Err(L"[ERROR] Failed to allocate memory for file info.\r\n");
    }

    status = FileHandle->GetInfo(FileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
    if (EFI_ERROR(status)){
        Free(FileInfo);
        Err(L"[ERROR] GetInfo (actual) failed.\r\n");
    }

    return FileInfo;
}

UINT64 FileSize(EFI_FILE_HANDLE FileHandle) {
    EFI_FILE_INFO *FileInfo = GetFileInfo(FileHandle);
    UINT64 size = FileInfo->FileSize;
    Free(FileInfo);
    return size;
}

void* ReadFile(EFI_FILE_HANDLE Volume, CHAR16 *Path) {
    EFI_FILE_HANDLE FileHandle;
    uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, Path, EFI_FILE_MODE_READ, 0);

    UINT64 ReadSize = FileSize(FileHandle);
    UINT8 *Buffer = Allocate(ReadSize);
    uefi_call_wrapper(FileHandle->Read, 3, FileHandle, &ReadSize, Buffer);

    uefi_call_wrapper(FileHandle->Close, 1, FileHandle);

    return Buffer;
}