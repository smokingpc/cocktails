// StorageHBAFirmwareUpdate.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include <ntddscsi.h>

#define BUFFER_SIZE 4096

FORCEINLINE int GetWritableSlotID(PSTORAGE_FIRMWARE_INFO info)
{
    for (int i = 0; i < info->SlotCount; i++)
    {
        if (FALSE == info->Slot[i].ReadOnly)
            return i;
    }

    return -1;
}
FORCEINLINE void FillHeaders(UCHAR* buffer, DWORD buffer_size, DWORD req_flag, UCHAR** following_data_buffer)
{
    PSRB_IO_CONTROL srb_ioctl = (PSRB_IO_CONTROL)buffer;
    PFIRMWARE_REQUEST_BLOCK request = (PFIRMWARE_REQUEST_BLOCK)(srb_ioctl + 1);

    //
    // The STORAGE_FIRMWARE_INFO is located after SRB_IO_CONTROL and FIRMWARE_RESQUEST_BLOCK
    // The offset should be aligned to pointer size!
    //
    ULONG offset = ((sizeof(SRB_IO_CONTROL) + sizeof(FIRMWARE_REQUEST_BLOCK) - 1) / sizeof(PVOID) + 1) * sizeof(PVOID);

    srb_ioctl->HeaderLength = sizeof(SRB_IO_CONTROL);
    srb_ioctl->ControlCode = IOCTL_SCSI_MINIPORT_FIRMWARE;
    RtlMoveMemory(srb_ioctl->Signature, IOCTL_MINIPORT_SIGNATURE_FIRMWARE, 8);
    srb_ioctl->Timeout = 30;
    srb_ioctl->Length = buffer_size - sizeof(SRB_IO_CONTROL);

    //
    // Set firmware request fields for FIRMWARE_FUNCTION_GET_INFO. This request is to the controller so
    // FIRMWARE_REQUEST_FLAG_CONTROLLER is set in the flags
    //
    request->Version = FIRMWARE_REQUEST_BLOCK_STRUCTURE_VERSION;
    request->Size = sizeof(FIRMWARE_REQUEST_BLOCK);
    request->Function = req_flag;
    request->Flags = FIRMWARE_REQUEST_FLAG_CONTROLLER;
    request->DataBufferOffset = offset;
    request->DataBufferLength = buffer_size - offset;

    *following_data_buffer = (buffer + offset);
}
FORCEINLINE void PrintSrbIoctlReturnCode(PSRB_IO_CONTROL srb_ioctl)
{
    switch (srb_ioctl->ReturnCode) {
    case FIRMWARE_STATUS_SUCCESS:
        printf("\t FirmwareUpgrade - firmware activate succeeded.\n");
        break;

    case FIRMWARE_STATUS_POWER_CYCLE_REQUIRED:
        printf("\t FirmwareUpgrade - firmware activate succeeded. PLEASE REBOOT COMPUTER.\n");
        break;

    case FIRMWARE_STATUS_ILLEGAL_REQUEST:
    case FIRMWARE_STATUS_INVALID_PARAMETER:
    case FIRMWARE_STATUS_INPUT_BUFFER_TOO_BIG:
        printf("\t FirmwareUpgrade - firmware activate parameter error. srbControl->ReturnCode %d.\n", srb_ioctl->ReturnCode);
        break;

    case FIRMWARE_STATUS_INVALID_SLOT:
        printf("\t FirmwareUpgrade - firmware activate, slot number invalid.\n");
        break;

    case FIRMWARE_STATUS_INVALID_IMAGE:
        printf("\t FirmwareUpgrade - firmware activate, invalid firmware image.\n");
        break;

    case FIRMWARE_STATUS_ERROR:
    case FIRMWARE_STATUS_CONTROLLER_ERROR:
        printf("\t FirmwareUpgrade - firmware activate, error returned.\n");
        break;

    default:
        printf("\t FirmwareUpgrade - firmware activate, unexpected error. srbControl->ReturnCode %d.\n", srb_ioctl->ReturnCode);
        break;
    }
}
FORCEINLINE void PrintUsage()
{
    printf("================================\n");
    printf("Usage : StorageHBAFirmwareUpdate.exe <Target Physical Disk> <firmware fullpath to load>\n");
    printf("Example : StorageHBAFirmwareUpdate.exe PhysicalDrive1 c:\\my_file\\new_firmware.bin\n");
    printf("================================\n");
}

BOOL DeviceGetFirmwareInfo(HANDLE device, UCHAR *buffer, DWORD buffer_size, PSTORAGE_FIRMWARE_INFO *out_info)
{
    PSRB_IO_CONTROL         srb_ioctl = (PSRB_IO_CONTROL)buffer;
    PFIRMWARE_REQUEST_BLOCK request = (PFIRMWARE_REQUEST_BLOCK)(srb_ioctl + 1);
    PSTORAGE_FIRMWARE_INFO  info = NULL;
    DWORD ret_size = 0;

    UCHAR* blob = NULL;
    FillHeaders(buffer, buffer_size, FIRMWARE_FUNCTION_GET_INFO, &blob);

    ULONG offset = ((sizeof(SRB_IO_CONTROL) + sizeof(FIRMWARE_REQUEST_BLOCK) - 1) / sizeof(PVOID) + 1) * sizeof(PVOID);
    blob = buffer + offset;
    *out_info = (PSTORAGE_FIRMWARE_INFO)blob;
    //
    // Send the request to get the device firmware info
    //
    BOOL ok = DeviceIoControl(device, IOCTL_SCSI_MINIPORT,
                            buffer, buffer_size,
                            buffer, buffer_size,
                            &ret_size, NULL);

    PrintSrbIoctlReturnCode(srb_ioctl);
    return ok;
}
void ShowDeviceFirmwareInfo(PSTORAGE_FIRMWARE_INFO info)
{
    printf("\t ----Firmware Information----\n");
    printf("\t Support upgrade command: %s\n", (info->UpgradeSupport ? "Yes" : "No"));
    printf("\t Slot Count: %d\n", info->SlotCount);
    printf("\t Current Active Slot: %d\n", info->ActiveSlot);
    if (info->PendingActivateSlot == STORAGE_FIRMWARE_INFO_INVALID_SLOT) 
    {
        printf("\t Pending Active Slot: %s\n\n", "N/A");
    }
    else 
    {
        printf("\t Pending Active Slot: %d\n\n", info->PendingActivateSlot);
    }

    for (int i = 0; i < info->SlotCount; i++)
    {
        char   revision[16] = { 0 };
        RtlCopyMemory(revision, &info->Slot[i].Revision.AsUlonglong, 8);

        printf("\t\t Slot Number: %d\n", info->Slot[i].SlotNumber);
        printf("\t\t Slot Read Only: %s\n", (info->Slot[i].ReadOnly ? "Yes" : "No"));
        printf("\t\t Revision: %s\n", revision);
        printf("\n");
    }

}
BOOL DeviceUploadFirmware(HANDLE device, char *filename, ULONG slot_id, UCHAR *buffer, DWORD buffer_size)
{
    BOOL ok = TRUE;
    PSRB_IO_CONTROL srb_ioctl = (PSRB_IO_CONTROL)buffer;
    UCHAR *blob = NULL;
    FillHeaders(buffer, buffer_size, FIRMWARE_FUNCTION_DOWNLOAD, &blob);
    PSTORAGE_FIRMWARE_DOWNLOAD download = (PSTORAGE_FIRMWARE_DOWNLOAD)blob;
    blob = blob + sizeof(STORAGE_FIRMWARE_DOWNLOAD);

    HANDLE file = CreateFileA(filename, GENERIC_READ,
        FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);

    if (INVALID_HANDLE_VALUE == file)
        return FALSE;

    DWORD file_size = GetFileSize(file, NULL);
    ULONG file_offset = 0;
    ULONG read_size = buffer_size - (blob - buffer);
    ULONG upload_size = 0;
    DWORD ret_size = 0;
    while(file_size > 0)
    {
        ZeroMemory(blob, read_size);
        if(TRUE == ReadFile(file, blob, read_size, &upload_size, NULL))
        {
            download->Version = 1;
            download->Size = sizeof(STORAGE_FIRMWARE_DOWNLOAD);
            download->Offset = file_offset;
            download->BufferSize = upload_size;

            ok = DeviceIoControl(device,
                IOCTL_SCSI_MINIPORT, buffer, buffer_size,
                buffer, buffer_size,
                &ret_size, NULL);

            if(FALSE == ok || srb_ioctl->ReturnCode != FIRMWARE_STATUS_SUCCESS)
            {
                if(file_size > 0)
                {
                    printf("Upload firmware failed. Offset=%d, SizeLeft=%d, ReturnCode=%X\n", file_offset, file_size, srb_ioctl->ReturnCode);
                    ok = FALSE;
                    goto exit;
                }
            }
        }
        else
            break;
        file_offset += upload_size;
        file_size -= upload_size;
    }

    ok = TRUE;
    printf("Upload firmware complete!\n");
exit:
    if(file != INVALID_HANDLE_VALUE)
        CloseHandle(file);
    
    return ok;
}
BOOL DeviceActivateSlot(HANDLE device, UCHAR* buffer, DWORD buffer_size, ULONG slot_id)
{
    PSRB_IO_CONTROL         srb_ioctl = (PSRB_IO_CONTROL)buffer;
    PFIRMWARE_REQUEST_BLOCK request = (PFIRMWARE_REQUEST_BLOCK)(srb_ioctl + 1);
    PSTORAGE_FIRMWARE_INFO  info = NULL;
    UCHAR* blob = NULL;
    DWORD ret_size = 0;

    FillHeaders(buffer, buffer_size, FIRMWARE_FUNCTION_ACTIVATE, &blob);
    PSTORAGE_FIRMWARE_ACTIVATE activate = (PSTORAGE_FIRMWARE_ACTIVATE) blob;
    activate->Version = 1;
    activate->Size = sizeof(STORAGE_FIRMWARE_ACTIVATE);
    activate->SlotToActivate = (UCHAR)slot_id;

    BOOL ok = DeviceIoControl(device, IOCTL_SCSI_MINIPORT,
                        buffer, buffer_size,
                        buffer, buffer_size,
                        &ret_size, NULL);

    PrintSrbIoctlReturnCode(srb_ioctl);
    return ok;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        PrintUsage();
        return -1;
    }

    char devname[128] = { 0 };
    _snprintf_s(devname, 128, 127, "\\\\.\\%s", argv[1]);
    char *filename = argv[2];
    UCHAR* buffer = NULL;
    //Steps of Update HBA Firmware:
    //1. list all firmware slots
    //2. select a suitable firmware slot to write
    //3. upload firmware file to specified slot
    //4. activate this slot to run firmware

    HANDLE hDevice = CreateFileA(devname, 0,    //to query storage property, you don't need R/W permission
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hDevice != INVALID_HANDLE_VALUE)
    {
        UCHAR buffer[BUFFER_SIZE] = {0};
        ZeroMemory(buffer, BUFFER_SIZE);
        PSTORAGE_FIRMWARE_INFO info = NULL;
        BOOL ok = DeviceGetFirmwareInfo(hDevice, buffer, BUFFER_SIZE, &info);

        if(FALSE == ok)
        {
            printf("Get device firmware info failed. LastError=%d\n", GetLastError());
            goto end;
        }

        ShowDeviceFirmwareInfo(info);
        if (FALSE == info->UpgradeSupport)
        {
            printf("This HBA doesn't support firmware update. EXIT...\n");
            goto end;
        }
        int slot_id = GetWritableSlotID(info);
        if (-1 == slot_id)
        {
            printf("No writable slot found. EXIT...\n");
            goto end;
        }

        ZeroMemory(buffer, BUFFER_SIZE);
        ok = DeviceUploadFirmware(hDevice, filename, slot_id, buffer, BUFFER_SIZE);
        if (FALSE == ok)
        {
            printf("upload firmware to device failed. LastError=%d\n", GetLastError());
            goto end;
        }

        ZeroMemory(buffer, BUFFER_SIZE);
        ok = DeviceActivateSlot(hDevice, buffer, BUFFER_SIZE, slot_id);
        if (FALSE == ok)
        {
            printf("Activate Firmware Slot[%d] failed. LastError=%d\n", slot_id, GetLastError());
            goto end;
        }

    }
    else
        printf("open device failed, last_error = %d\n", GetLastError());

end:
    if (hDevice != INVALID_HANDLE_VALUE)
        CloseHandle(hDevice);
}
