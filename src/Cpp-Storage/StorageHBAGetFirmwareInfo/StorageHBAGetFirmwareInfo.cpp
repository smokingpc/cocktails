// StorageHBAGetFirmwareInfo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include <ntddscsi.h>

#define BUFFER_SIZE 4096

void PrintUsage()
{
    printf("================================\n");
    printf("Usage : StorageHBAGetFirmwareInfo.exe <Target Physical Disk>\n");
    printf("Example : StorageHBAGetFirmwareInfo.exe PhysicalDrive1 \n");
    printf("================================\n");
}

BOOL DeviceGetFirmwareInfo(HANDLE device, UCHAR* buffer, DWORD buffer_size)
{
    ULONG offset = 0;
    PSRB_IO_CONTROL         srb_ioctl = NULL;
    PFIRMWARE_REQUEST_BLOCK request = NULL;
    PSTORAGE_FIRMWARE_INFO  info = NULL;
    DWORD ret_size = 0;

    srb_ioctl = (PSRB_IO_CONTROL)buffer;
    request = (PFIRMWARE_REQUEST_BLOCK)(srb_ioctl + 1);

    //
    // The STORAGE_FIRMWARE_INFO is located after SRB_IO_CONTROL and FIRMWARE_RESQUEST_BLOCK
    // The offset should be aligned to pointer size!
    //
    offset = ((sizeof(SRB_IO_CONTROL) + sizeof(FIRMWARE_REQUEST_BLOCK) - 1) / sizeof(PVOID) + 1) * sizeof(PVOID);

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
    request->Function = FIRMWARE_FUNCTION_GET_INFO;
    request->Flags = FIRMWARE_REQUEST_FLAG_CONTROLLER;
    request->DataBufferOffset = offset;
    request->DataBufferLength = buffer_size - offset;

    //
    // Send the request to get the device firmware info
    //
    return DeviceIoControl(device, IOCTL_SCSI_MINIPORT,
        buffer, buffer_size,
        buffer, buffer_size,
        &ret_size, NULL);
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

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        PrintUsage();
        return -1;
    }

    char devname[128] = { 0 };
    _snprintf_s(devname, 128, 127, "\\\\.\\%s", argv[1]);

    HANDLE hDevice = CreateFileA(devname, 0,    //to query storage property, you don't need R/W permission
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hDevice != INVALID_HANDLE_VALUE)
    {
        UCHAR buffer[BUFFER_SIZE] = { 0 };
        BOOL ok = DeviceGetFirmwareInfo(hDevice, buffer, BUFFER_SIZE);

        if (FALSE == ok)
        {
            printf("Get device firmware info failed. LastError=%d\n", GetLastError());
            goto end;
        }
        PSRB_IO_CONTROL         srb_ioctl = (PSRB_IO_CONTROL)buffer;
        PFIRMWARE_REQUEST_BLOCK request = (PFIRMWARE_REQUEST_BLOCK)(srb_ioctl + 1);
        PSTORAGE_FIRMWARE_INFO  info = (PSTORAGE_FIRMWARE_INFO)(buffer + request->DataBufferOffset);

        ShowDeviceFirmwareInfo(info);
    }
    else
        printf("open device failed, last_error = %d\n", GetLastError());

end:
    if (hDevice != INVALID_HANDLE_VALUE)
        CloseHandle(hDevice);
}
