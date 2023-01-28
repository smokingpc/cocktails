#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
typedef struct _TEST_DATA1 {
    char Name[64];
}TEST_DATA1;

typedef struct _TEST_DATA2 {
    int Number;
}TEST_DATA2;

int main()
{
    TEST_DATA1 *ptr1 = NULL;
    TEST_DATA2 data = {0};
    data.Number = 12345678;
    strcat(ptr1->Name,"Test");
    printf("Name=%s\n", ptr1->Name);
}
