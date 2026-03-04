// boost_program_location_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem.hpp>

using namespace std;

#pragma comment(lib, "libboost_system-vc141-mt-sgd-1_65.lib")

int _tmain(int argc, TCHAR* argv[])
{
    boost::filesystem::path fullpath = boost::dll::program_location();
    boost::filesystem::path dir = fullpath.parent_path();

    //no "\\" on tail in dir variable.
    //output example : "current exe path = F:\\my_github\\Cocktails\\src\\output\\Debug\\Win32"
    printf("current exe path = %s\n", dir.string().c_str());
}
