// DumpPhysicalDiskData.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <string>

#define SIZE_1KB    1024
#define SIZE_1MB    (SIZE_1KB*SIZE_1KB)
#define SIZE_256KB  (SIZE_1KB*256)
#define SIZE_4KB    (4*SIZE_1KB)

void Usage()
{
    printf("Usage: DumpPhysicalDiskData.exe [physical_drive_devpath] [begin_offset_in_4KB] [dump_size_in_4KB] [output_file_fullpath]\n");
    printf("e.g.: If you want to dump first 3MB of physical disk 1 to c:\\test\\dumpdata.bin ,\n");
    printf("      you should use command: DumpPhysicalDiskData.exe \\\\.\\PhysicalDrive1 0 3072 c:\\test\\dumpdata.bin \n\n");
}

void DumpDataFromPhysicalDisk(char *disk_devpath, LARGE_INTEGER offset, UINT32 dump_size_4kb, char *output_file)
{
    HANDLE disk = CreateFileA(disk_devpath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    HANDLE file = CreateFileA(output_file, GENERIC_WRITE, FILE_SHARE_READ , NULL, CREATE_ALWAYS, 0, NULL);
    int counter = 0;
    if(disk != INVALID_HANDLE_VALUE)
    {
        if(file != INVALID_HANDLE_VALUE)
        {
            printf("Try to set disk read offset(%lld)\n", offset.QuadPart);
            DWORD seek_err = SetFilePointer(disk, offset.LowPart, &offset.HighPart, FILE_BEGIN);
            if(INVALID_SET_FILE_POINTER == seek_err)
            {
                printf("SeekFile failed, LastError(%d)\n", GetLastError());
            }
            else
            {
                printf("begin to dump data from (%s) offset(%llX), dump size(%X in 4KB)\n",
                    disk_devpath, offset.QuadPart, dump_size_4kb);
                for (int i = 0; i < dump_size_4kb; i++)
                {
                    BYTE buffer[SIZE_4KB] = { 0 };
                    DWORD ret_size = 0;
                    DWORD written_size = 0;
                    BOOL ok = ReadFile(disk, buffer, SIZE_4KB, &ret_size, NULL);
                    if (!ok || 0 == ret_size)
                    {
                        printf("Read disk failed, LastError=%d\n", GetLastError());
                        break;
                    }
                    printf("Offset(%llX) read , write to file.\n",
                        offset.QuadPart + i * SIZE_4KB);
                    ok = WriteFile(file, buffer, ret_size, &written_size, NULL);
                    if (!ok)
                    {
                        printf("Write to file failed, LastError=%d\n", GetLastError());
                        break;
                    }

                    counter++;
                }
            }
            CloseHandle(file);
        }
        else
            printf("Open File(%s) FAILED!!\n", output_file);
        CloseHandle(disk);
    }
    else
        printf("Open Disk(%s) FAILED!!\n", disk_devpath);

    printf("Total Read/Write data size = %d blocks(1 block = 4KB)\n", counter);
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        Usage();
        return -1;
    }

    //physical disk symbolic link name, e.g. "\\.\PhysicalDrive4"
    std::string devpath = argv[1];
    int64_t offset_4kb = _atoi64(argv[2]);
    int size_4kb = atoi(argv[3]);
    std::string filepath = argv[4];
    LARGE_INTEGER offset = {0};
    offset.QuadPart = offset_4kb * SIZE_4KB;
    printf("Try to dump %s offset(%lld) size(%d)\n", 
        devpath.c_str(), offset.QuadPart, size_4kb*SIZE_4KB);

    DumpDataFromPhysicalDisk(
            (char*)devpath.c_str(), 
            offset, 
            size_4kb, 
            (char*)filepath.c_str());
}

