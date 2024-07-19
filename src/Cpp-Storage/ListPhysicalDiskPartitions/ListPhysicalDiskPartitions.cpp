// ListPhysicalDiskPartitions.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include <string>
#include <iostream>

#define SIZE_1MB    1048576
typedef std::basic_string<TCHAR> tstring;
using namespace std;

TCHAR* ParseGptAttribute(DWORD64 attribute)
{
    switch (attribute)
    {
    case 0:
        return (TCHAR*)_T("NORMAL (0)");
    case GPT_ATTRIBUTE_PLATFORM_REQUIRED:
        return (TCHAR*)_T("PLATFORM_REQUIRED");
    case GPT_ATTRIBUTE_NO_BLOCK_IO_PROTOCOL:
        return (TCHAR*)_T("NO_BLOCK_IO_PROTOCOL");
    case GPT_ATTRIBUTE_LEGACY_BIOS_BOOTABLE:
        return (TCHAR*)_T("LEGACY_BIOS_BOOTABLE");
    case GPT_BASIC_DATA_ATTRIBUTE_NO_DRIVE_LETTER:
        return (TCHAR*)_T("NO_DRIVE_LETTER");
    case GPT_BASIC_DATA_ATTRIBUTE_HIDDEN:
        return (TCHAR*)_T("HIDDEN");
    case GPT_BASIC_DATA_ATTRIBUTE_SHADOW_COPY:
        return (TCHAR*)_T("SHADOW_COPY");
    case GPT_BASIC_DATA_ATTRIBUTE_READ_ONLY:
        return (TCHAR*)_T("READ_ONLY");
    case GPT_BASIC_DATA_ATTRIBUTE_OFFLINE:
        return (TCHAR*)_T("OFFLINE");
    case GPT_BASIC_DATA_ATTRIBUTE_DAX:
        return (TCHAR*)_T("DAX");
    case GPT_BASIC_DATA_ATTRIBUTE_SERVICE:
        return (TCHAR*)_T("SERVICE");
        break;
    }
    return NULL;
}

inline TCHAR* ParseMbrPartitionType(BYTE type)
{
    switch(type)
    {
    case PARTITION_ENTRY_UNUSED:
        return (TCHAR*)_T("UNUSED");
    case PARTITION_EXTENDED:
        return (TCHAR*)_T("EXTENDED");
    case PARTITION_FAT_12:
        return (TCHAR*)_T("FAT12");
    case PARTITION_FAT_16:
        return (TCHAR*)_T("FAT16");
    case PARTITION_FAT32:
        return (TCHAR*)_T("FAT32");
    case PARTITION_IFS:
        return (TCHAR*)_T("IFS");
    case PARTITION_LDM:
        return (TCHAR*)_T("LDM");
    case PARTITION_NTFT:
        return (TCHAR*)_T("NTFT");
    case VALID_NTFT:
        return (TCHAR*)_T("VALID_NTFT");
    }

    return (TCHAR*)_T("UNKNOWN");
}
inline void ParseGptAttribute(tstring &result, DWORD64 attr)
{
    TCHAR *parse = NULL;
    parse = ParseGptAttribute(attr & GPT_ATTRIBUTE_PLATFORM_REQUIRED);
    if(NULL!= parse)
        result += parse;
    parse = ParseGptAttribute(attr & GPT_ATTRIBUTE_NO_BLOCK_IO_PROTOCOL);
    if (NULL != parse)
        result += parse;
    parse = ParseGptAttribute(attr & GPT_ATTRIBUTE_LEGACY_BIOS_BOOTABLE);
    if (NULL != parse)
        result += parse;
    parse = ParseGptAttribute(attr & GPT_BASIC_DATA_ATTRIBUTE_NO_DRIVE_LETTER);
    if (NULL != parse)
        result += parse;
    parse = ParseGptAttribute(attr & GPT_BASIC_DATA_ATTRIBUTE_HIDDEN);
    if (NULL != parse)
        result += parse;
    parse = ParseGptAttribute(attr & GPT_BASIC_DATA_ATTRIBUTE_SHADOW_COPY);
    if (NULL != parse)
        result += parse;
    parse = ParseGptAttribute(attr & GPT_BASIC_DATA_ATTRIBUTE_READ_ONLY);
    if (NULL != parse)
        result += parse;
    parse = ParseGptAttribute(attr & GPT_BASIC_DATA_ATTRIBUTE_OFFLINE);
    if (NULL != parse)
        result += parse;
    parse = ParseGptAttribute(attr & GPT_BASIC_DATA_ATTRIBUTE_DAX);
    if (NULL != parse)
        result += parse;
    parse = ParseGptAttribute(attr & GPT_BASIC_DATA_ATTRIBUTE_SERVICE);
    if (NULL != parse)
        result += parse;
}

void PrintGptLayoutInfo(DRIVE_LAYOUT_INFORMATION_GPT* gpt)
{
    _tprintf(_T("DiskID=%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"), 
        gpt->DiskId.Data1, gpt->DiskId.Data2, gpt->DiskId.Data3, 
        gpt->DiskId.Data4[0], gpt->DiskId.Data4[1], gpt->DiskId.Data4[2], gpt->DiskId.Data4[3], 
        gpt->DiskId.Data4[4], gpt->DiskId.Data4[5], gpt->DiskId.Data4[6], gpt->DiskId.Data4[7]);
    _tprintf(_T("StartUsableOffset=0x%016llX bytes, UsableLength=0x%016llX bytes\n"), gpt->StartingUsableOffset.QuadPart, gpt->UsableLength.QuadPart);
    _tprintf(_T("MaxPartitionCount=0x%08X\n"), gpt->MaxPartitionCount);
}
void PrintMbrLayoutInfo(DRIVE_LAYOUT_INFORMATION_MBR* mbr)
{
    _tprintf(_T("Checksum=0x%08X, Signature=0x%08X\n"), mbr->CheckSum, mbr->Signature);
}

void PrintPartitionInfoGpt(PARTITION_INFORMATION_GPT *gpt)
{
    _tprintf(_T("\tPartitionType=%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"),
        gpt->PartitionType.Data1, gpt->PartitionType.Data2, gpt->PartitionType.Data3,
        gpt->PartitionType.Data4[0], gpt->PartitionType.Data4[1], gpt->PartitionType.Data4[2], gpt->PartitionType.Data4[3],
        gpt->PartitionType.Data4[4], gpt->PartitionType.Data4[5], gpt->PartitionType.Data4[6], gpt->PartitionType.Data4[7]);

    _tprintf(_T("\tPartitionId=%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"),
        gpt->PartitionId.Data1, gpt->PartitionId.Data2, gpt->PartitionId.Data3,
        gpt->PartitionId.Data4[0], gpt->PartitionId.Data4[1], gpt->PartitionId.Data4[2], gpt->PartitionId.Data4[3],
        gpt->PartitionId.Data4[4], gpt->PartitionId.Data4[5], gpt->PartitionId.Data4[6], gpt->PartitionId.Data4[7]);
    tstring attribute = _T("");
    ParseGptAttribute(attribute, gpt->Attributes);
    _tprintf(_T("\tAttribute=%s\n"), attribute.c_str());
    _tprintf(_T("\tName=%s\n"), gpt->Name);
}
void PrintPartitionInfoMbr(PARTITION_INFORMATION_MBR* mbr) 
{
    _tprintf(_T("\tPartitionType=%s, BootIndicator=%d \n"), ParseMbrPartitionType(mbr->PartitionType), mbr->BootIndicator);
    _tprintf(_T("\tRecognizedPartition=%d, HiddenSectors=0x%08X\n"), mbr->RecognizedPartition, mbr->HiddenSectors);
    _tprintf(_T("\tPartitionId=%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"),
        mbr->PartitionId.Data1, mbr->PartitionId.Data2, mbr->PartitionId.Data3, 
        mbr->PartitionId.Data4[0], mbr->PartitionId.Data4[1], mbr->PartitionId.Data4[2], mbr->PartitionId.Data4[3], 
        mbr->PartitionId.Data4[4], mbr->PartitionId.Data4[5], mbr->PartitionId.Data4[6], mbr->PartitionId.Data4[7]);
}
void PrintPartitionEntries(PARTITION_INFORMATION_EX* partition, DWORD count)
{
    for(int i=0; i<count; i++)
    {
        PARTITION_INFORMATION_EX* data = &partition[i];
        TCHAR *style = NULL;
        
        switch (data->PartitionStyle)
        {
        case PARTITION_STYLE_MBR:
            style=(TCHAR*)_T("MBR");
            break;
        case PARTITION_STYLE_GPT:
            style = (TCHAR*)_T("GPT");
            break;
        case PARTITION_STYLE_RAW:
            style = (TCHAR*)_T("RAW");
            break;
        }
        //PARTITION_INFORMATION_EX::PartitionNumber is 1-based number
        _tprintf(_T("\t[%s Partition-%d]\n"), style, i);
        _tprintf(_T("\tPartitionNumber=%d, Rewritable=%d, IsService=%d\n"), data->PartitionNumber, data->RewritePartition, data->IsServicePartition);
        _tprintf(_T("\tStartOffset=0x%016llX bytes, PartitionLength=0x%016llX bytes\n"), data->StartingOffset.QuadPart, data->PartitionLength.QuadPart);

        switch (data->PartitionStyle)
        {
        case PARTITION_STYLE_MBR:
            PrintPartitionInfoMbr(&data->Mbr);
            break;
        case PARTITION_STYLE_GPT:
            PrintPartitionInfoGpt(&data->Gpt);
            break;
        }
    }
}
void PrintDiskPartitions(DRIVE_LAYOUT_INFORMATION_EX* info)
{
    switch(info->PartitionStyle)
    {
        case PARTITION_STYLE_MBR:
            _tprintf(_T("PartitionStyle=%s, PartitionCount=%d\n"), _T("MBR"), info->PartitionCount);
            _tprintf(_T("\n"));
            PrintMbrLayoutInfo(&info->Mbr);
            _tprintf(_T("\n"));
            break;
        case PARTITION_STYLE_GPT:
            _tprintf(_T("PartitionStyle=%s, PartitionCount=%d\n"), _T("GPT"), info->PartitionCount);
            _tprintf(_T("\n"));
            PrintGptLayoutInfo(&info->Gpt);
            _tprintf(_T("\n"));
            break;
        case PARTITION_STYLE_RAW:
            _tprintf(_T("PartitionStyle=%s, PartitionCount=%d\n"), _T("**RAW"), info->PartitionCount);
            _tprintf(_T("\n"));
            break;
    }

    PrintPartitionEntries(info->PartitionEntry, info->PartitionCount);
}

//devpath => physical disk devpath
// e.g.  "\\.\PhysicalDrive4"
BOOL GetPartitionList(IN OUT BYTE *buffer, size_t buf_size, tstring &devpath)
{
    DWORD ret_size = 0;
    BOOL ok = FALSE;
    HANDLE disk = CreateFile(devpath.c_str(), GENERIC_ALL, FILE_SHARE_READ| FILE_SHARE_WRITE,
                            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(INVALID_HANDLE_VALUE != disk)
    {
    //IOCTL_DISK_GET_DRIVE_LAYOUT_EX won't tell you "how big of buffer it need."
    // so we have to allocate a huge buffer which big enough to query...
        //ok = DeviceIoControl(disk, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, NULL, 0, &ret_size, NULL);
        //if(!ok)
        //{
        //    _tprintf(_T("IOCTL_DISK_GET_DRIVE_LAYOUT_EX requires buffer size=%d, last error=%d\n"), ret_size, GetLastError());
        //    return FALSE;
        //}

        ok = DeviceIoControl(disk, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, buffer, buf_size, &ret_size, NULL);
        
        if(!ok)
        {
            _tprintf(_T("IOCTL_DISK_GET_DRIVE_LAYOUT_EX failed, last error=%d\n"), GetLastError());
        }

        CloseHandle(disk);
    }
    else
        _tprintf(_T("Open physical disk [%s] failed. LastError=%d\n"), devpath.c_str(), GetLastError());

    return ok;
}

void Usage()
{
    _tprintf(_T("Usage: ListPhysicalDiskPartitions.exe [physical_drive_devpath]\n"));
    _tprintf(_T("e.g.: If you want to list all partitions in physical disk3 ,\n"));
    _tprintf(_T("      execute this tool as : ListPhysicalDiskPartitions.exe \\\\.\\PhysicalDrive3 \n"));
}

int _tmain(int argc, TCHAR* argv[])
{
    if(argc<2)
    {
        Usage();
        return -1;
    }

    tstring devpath = argv[1];
    BYTE *buffer = new BYTE[SIZE_1MB];
    RtlZeroMemory(buffer, SIZE_1MB);
    BOOL ok = GetPartitionList(buffer, SIZE_1MB, devpath);
    if(ok)
    {
        _tprintf(_T("[Drive Layout Info of %s]\n"), devpath.c_str());
        DRIVE_LAYOUT_INFORMATION_EX* info = (DRIVE_LAYOUT_INFORMATION_EX *)buffer;
        PrintDiskPartitions(info);
    }
    delete [] buffer;
}