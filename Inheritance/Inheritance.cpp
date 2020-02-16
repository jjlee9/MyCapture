// Inheritance.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>

class Base
{
public:
    virtual ~Base()
    {
        if (commit_) { return; }
        // Might not work!
        undo();
    }

    virtual void commit() { commit_ = true; }
    virtual void undo()
    {
        std::cout << "Base::undo()\n";
    }

protected:
    bool commit_ = false;
};

class Derived : public Base
{
public:
    virtual ~Derived() override
    {
        if (commit_) { return; }
        undo();
    }

    virtual void undo()
    {
        std::cout << "Derived::undo()\n";
    }
};

class Shape
{
public:
    virtual ~Shape()
    {
        if (commit_) { return; }
        undo();
    };

    virtual void commit() { commit_ = true; }
    virtual void undo()
    {
        std::cout << "Shape::undo()\n";
    }

protected:
    bool commit_ = false;
};

class Rectangle : public Shape
{
public:
    virtual ~Rectangle() override
    {
        if (commit_) { return; }
        // Might not work
        undo();
    }

    virtual void undo() override
    {
        std::cout << "Rectangle::undo() I want to XOR the background to undo the movement\n";
    }
};

// not a good design
class Square : public Rectangle
{
public:
    virtual ~Square() override
    {
        if (commit_) { return; }
        // Might not work
        undo();
    }

    virtual void undo() override
    {
        std::cout << "Square::undo() I want to XOR the background to undo the movement\n";
    }
};

class Shape2
{
protected:
    virtual ~Shape2() {}

    virtual void commit() { commit_ = true; }
    void undo()
    {
        std::cout << "Shape2::undo()\n";
    }

protected:
    bool commit_ = false;
};

class Rectangle2 : public Shape2
{
protected:
    void undo()
    {
        std::cout << "Rectangle2::undo() I want to XOR the background to undo the movement\n";
    }
};

class Square2 : public Rectangle2
{
protected:
    void undo()
    {
        std::cout << "Square2::undo() I want to XOR the background to undo the movement\n";
    }
};

template <class T> class CComObject : public T
{
public:
    ~CComObject()
    {
        // undo(); // compiler error, no undo in CComObject
        // static_cast<T*>(this)->undo(); // compiler error, undo is protected
        static_cast<CComObject<T>*>(this)->undo();
    }
};

struct InterfaceX
{
    virtual void MethodX1() = 0;
    virtual void MethodX2() = 0;
};

struct InterfaceY
{
    virtual void MethodY1() = 0;
    virtual void MethodY2() = 0;
};

class MyClass : public InterfaceX, public InterfaceY
{
public:
    virtual void MethodX1() override
    {
        std::cout << "MethodX1\n";
    }

    virtual void MethodX2() override
    {
        std::cout << "MethodX2\n";
    }

    virtual void MethodY1() override
    {
        std::cout << "MethodY1\n";
    }

    virtual void MethodY2() override
    {
        std::cout << "MethodY2\n";
    }

protected:
    std::string u;
    std::string v;
};

int main()
{
    std::cout << "Check Derived\n";
    {
        auto spDerived = std::make_unique<Derived>();
    }
    std::cout << "\n";

    std::cout << "Check Square\n";
    {
        auto spSquare = std::make_unique<Square>();
    }
    std::cout << "\n";

    std::cout << "Check CComObject<Square2>\n";
    {
        auto spComObjSquare2 = std::make_unique<CComObject<Square2>>();
    }
    std::cout << "\n";

    std::cout << "Check MyClass\n";
    {
        auto spMyClass = std::make_unique<MyClass>();
        auto pMyClass = spMyClass.get();
        spMyClass->MethodX1();
        spMyClass->MethodX2();
        spMyClass->MethodY1();
        spMyClass->MethodY2();
    }
    std::cout << "\n";
}
