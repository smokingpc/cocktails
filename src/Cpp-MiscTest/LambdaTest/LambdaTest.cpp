// LambdaTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
using namespace std;

//lambda format:
// [ captures ] ( params in lambda ) -> ret { body }
// [<%parent arg passing%>] (lambda args declation) <%postfix> -> return_type { }

int main()
{
    int data = 7;
    {
        auto func1 = [] (int a, int b) {
            cout << "Lambda1: a+b=" << a + b << endl << endl;
        };
        func1(1, 2);
    }
    {
        int x=3;
        auto func2 = [=]() {
            //x can't be modified if pass by value.
            cout << "Lambda2: x*x=" << x*x << endl << endl;
        };
        func2();
    }
    
    {
        int c=3;
        auto func3 = [&]() {
            cout << "Lambda3: c=" << c++ << endl;
        };
        func3();
        cout << "After Lambda3: c=" << c << endl << endl;
    }

    //capture external variable is limited to variable lifescope.
    {
        int d = 9;
        auto func4 = [=]() -> int {
        //capture data is ok, but can't capture x and c
            cout << "Lambda4: data=" << data << endl;
            cout << "Lambda4: d=" << d << endl;
            return d+1;
        };
        cout << "Lambda4 returned: " << func4() << endl << endl;
    }

    {
        int e = 8;
        //no need to declare type of foo, 
        auto func5 = [foo=2, e]() {
            cout << "Lambda5: foo * e=" << foo * e << endl << endl;
        };
        func5();
    }

    {
        auto func6 = [](auto s, auto t) {
            cout << "Lambda6: s / t = " << s / t << endl << endl;
        };
        func6(32, 8);
    }

    {
    //if using auto, type of arg should support operator used in lambda
        auto func7 = [](auto o, auto p) {
            cout << "Lambda7: o + p = " << o + p << endl << endl;
        };
        //func7("foo", "bar");  //invalid usage
        string o = "foo";
        string p = "bar";
        func7(o, p);
    }

    {
        string o2 = "foo2 ";
        string p2 = "bar2";
        auto func8 = [=]() {
            //capture [=] will construct new object of std::string
            cout << "Lambda8: o2 + p2 = " << o2 + p2 << endl << endl;
        };
        func8();
    }
}

