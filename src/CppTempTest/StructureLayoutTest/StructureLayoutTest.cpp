// StructureLayoutTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include <string>
#include <iostream>

int _tmain(int argc, TCHAR* argv[])
{
    DRIVE_LAYOUT_INFORMATION_EX info = {0};
    DRIVE_LAYOUT_INFORMATION_MBR mbr_info = {0};
    DRIVE_LAYOUT_INFORMATION_GPT gpt_info = { 0 };
    PARTITION_INFORMATION_EX part_info;
    PARTITION_INFORMATION_MBR part_mbr = {0};
    PARTITION_INFORMATION_GPT part_gpt = { 0 };

    PVOID base_ptr = (PVOID)(ULONG_PTR) &info;
    _tprintf(_T("[DRIVE_LAYOUT_INFORMATION_EX]\n"));
    _tprintf(_T("\tPartitionStyle(%lu)\n"), (ULONG_PTR)&info.PartitionStyle - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tPartitionCount(%lu)\n"), (ULONG_PTR)&info.PartitionCount - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tGpt/Mbr (%lu)\n"), (ULONG_PTR)&info.Gpt - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tPartitionEntry(%lu)\n"), (ULONG_PTR)info.PartitionEntry - (ULONG_PTR)base_ptr);
    _tprintf(_T("\n"));

    base_ptr = (PVOID)&mbr_info;
    _tprintf(_T("[DRIVE_LAYOUT_INFORMATION_MBR]\n"));
    _tprintf(_T("\tSignature(%lu)\n"), (ULONG_PTR)&mbr_info.Signature - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tCheckSum(%lu)\n"), (ULONG_PTR)&mbr_info.CheckSum - (ULONG_PTR)base_ptr);
    _tprintf(_T("\n"));

    base_ptr = (PVOID)&gpt_info;
    _tprintf(_T("[DRIVE_LAYOUT_INFORMATION_GPT]\n"));
    _tprintf(_T("\tDiskId(%lu)\n"), (ULONG_PTR)&gpt_info.DiskId - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tStartingUsableOffset(%lu)\n"), (ULONG_PTR)&gpt_info.StartingUsableOffset - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tUsableLength(%lu)\n"), (ULONG_PTR)&gpt_info.UsableLength - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tMaxPartitionCount(%lu)\n"), (ULONG_PTR)&gpt_info.MaxPartitionCount - (ULONG_PTR)base_ptr);
    _tprintf(_T("\n"));

    base_ptr = (PVOID)&part_info;
    _tprintf(_T("[PARTITION_INFORMATION_EX]\n"));
    _tprintf(_T("\tPartitionStyle(%lu)\n"), (ULONG_PTR)&part_info.PartitionStyle - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tStartingOffset(%lu)\n"), (ULONG_PTR)&part_info.StartingOffset - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tPartitionLength(%lu)\n"), (ULONG_PTR)&part_info.PartitionLength - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tPartitionNumber(%lu)\n"), (ULONG_PTR)&part_info.PartitionNumber - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tRewritePartition(%lu)\n"), (ULONG_PTR)&part_info.RewritePartition - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tIsServicePartition(%lu)\n"), (ULONG_PTR)&part_info.IsServicePartition - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tGpt/Mbr (%lu)\n"), (ULONG_PTR)&part_info.Gpt - (ULONG_PTR)base_ptr);
    _tprintf(_T("\n"));

    base_ptr = (PVOID)&part_mbr;
    _tprintf(_T("[PARTITION_INFORMATION_MBR]\n"));
    _tprintf(_T("\tPartitionType(%lu)\n"), (ULONG_PTR)&part_mbr.PartitionType - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tBootIndicator(%lu)\n"), (ULONG_PTR)&part_mbr.BootIndicator - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tRecognizedPartition(%lu)\n"), (ULONG_PTR)&part_mbr.RecognizedPartition - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tHiddenSectors(%lu)\n"), (ULONG_PTR)&part_mbr.HiddenSectors - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tPartitionId(%lu)\n"), (ULONG_PTR)&part_mbr.PartitionId - (ULONG_PTR)base_ptr);
    _tprintf(_T("\n"));

    base_ptr = (PVOID)&part_gpt;
    _tprintf(_T("[PARTITION_INFORMATION_GPT]\n"));
    _tprintf(_T("\tPartitionType(%lu)\n"), (ULONG_PTR)&part_gpt.PartitionType - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tPartitionId(%lu)\n"), (ULONG_PTR)&part_gpt.PartitionId - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tAttributes(%lu)\n"), (ULONG_PTR)&part_gpt.Attributes - (ULONG_PTR)base_ptr);
    _tprintf(_T("\tName(%lu)\n"), (ULONG_PTR)&part_gpt.Name - (ULONG_PTR)base_ptr);
    _tprintf(_T("\n"));
}
