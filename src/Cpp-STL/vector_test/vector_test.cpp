// vector_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<int> vector1;
    vector<string> vector2;

    //push and push_back "copy" the object into container.
    //emplace and emplace_back construct object in container space.
    //emplace family is faster because avoid "copy" action. 
    vector1.emplace_back(1);
    vector1.emplace_back(2);

    string str1 = "aaa";
    string str2 = "bbb";

    vector2.push_back(str1);
    vector2.emplace_back(str2);
}
