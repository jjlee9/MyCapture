// ConstOrMutable.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

class ConstOrMutable1
{
public:
    // Demo purpose only, we should not use new for modern C++.
    ConstOrMutable1() : pTest_(new std::string("J. J."))
    {
    }

    void Modify() const
    {
        (*pTest_) = "T. J.";
    }

private:
    std::string* pTest_;
};

class ConstOrMutable2
{
public:
    // Demo purpose only, we should not use new for modern C++.
    ConstOrMutable2() : test_("J. J.")
    {
    }

    void Modify() const // this method should be mutable!!!
    {
        test_ = "T. J.";
    }

private:
    std::string test_;
};

int main()
{
    ConstOrMutable1 constOrMutable1;
    constOrMutable1.Modify();

    ConstOrMutable2 constOrMutable2;
    constOrMutable2.Modify();
}
