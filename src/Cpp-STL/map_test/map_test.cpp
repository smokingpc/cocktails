// map_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <map>

using namespace std;

class CMyData {
    int id = -1;
    TCHAR name[16] = {0};
};

int _tmain(int argc, TCHAR* argv[])
{
//  map< index , data>
    map<int, float> map1;
    map<int, CMyData*> map2;
    map<int, CMyData> map3;

    map1[1] = 0.5;
    //if index not exist, map still create a element(in default value) with this new index
    float test = map1[2];   
    CMyData* test2 = map2[3];
    CMyData test3 = map3[4];
    CMyData test4 = map3[5];
}
