// BinaryDataInStrTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main()
{
	char temp1[5] = { 0x81, 0x82, 0x83, 0x84, 0};
	char temp2[8] = { 0x81, 0x42, 0x43, 0x84, 0, 0x31, 0x33, 0 };
	char temp3[9] = { 0x41, 0x42, 0, 0xa4, 0, 0x31, 0x33, 0xcc, 0 };
	string data1 = temp1;
	printf("5 bytes binary in string, str.size()=%lld\n", data1.size());
	data1.clear();
	printf("after clear, str.size()=%lld\n", data1.size());

	data1 = temp2;
	printf("8 bytes binary in string, str.size()=%lld\n", data1.size());
	data1.clear();
	printf("after clear, str.size()=%lld\n", data1.size());

	//both append() and assign() are ok.
	//data1.append(temp3, sizeof(temp3));
	data1.assign(temp3, sizeof(temp3));
	printf("9 bytes binary in string, str.size()=%lld\n", data1.size());

	size_t size = data1.size();
	const char* ptr = data1.data();
	for (size_t i = 0; i < size; i++)
	{
		if (0 == i % 4)
			printf("\n");
		printf("ptr[%lld]=%02X, ", i, (unsigned char) ptr[i]);
	}
}
