// SmartPointer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <memory>
#include <thread>

class Employee
{
public:
    explicit Employee(
        std::string id,
        std::string firstName,
        std::string lastName) noexcept :
        id_(std::move(id)),
        firstName_(std::move(firstName)),
        lastName_(std::move(lastName))
    {
    }

    // demo only, not a really good return type
    const std::string& Id() const { return id_; }

    // demo only, not a really good return type
    const std::string& FirstName() const { return firstName_; }

    // demo only, not a really good return type
    const std::string& LastName() const { return lastName_; }

    friend std::ostream& operator <<(std::ostream&, const Employee&);

protected:
    std::string id_;
    std::string firstName_;
    std::string lastName_;
};

std::ostream& operator <<(
    std::ostream& os,
    const Employee& emp)
{
    // setw for alignment
    os << "Id:" << std::setw(5)
        << emp.id_ << " Name:" << std::setw(5) << emp.firstName_ << " "
        << std::left << std::setw(5) << emp.lastName_
        << "\n";
    return os;
}

int main()
{
    auto spEmp = std::make_shared<Employee>("13579", "J. J.", "Lee");
    // Watch out! std::thread is not so OK for general usage.
    std::cout << "Lambda - capture smart pointer by value\n";

    std::thread thrd1([spEmp]() mutable
        {
            spEmp = std::make_shared<Employee>("12345", "T. J.", "Wang");
            std::cout << *spEmp;
        });
    if (thrd1.joinable())
    {
        thrd1.join();
    }

    std::cout << "Check original spEmp after captured by value\n";
    std::cout << *spEmp << "\n";

    std::cout << "Lambda - capture smart pointer by reference\n";
    std::thread thrd2([&spEmp]() mutable
        {
            spEmp = std::make_shared<Employee>("11223", "G. H.", "Chin");
            std::cout << *spEmp;
        });
    if (thrd2.joinable())
    {
        thrd2.join();
    }
    std::cout << "Check original spEmp after captured by refernce\n";
    std::cout << *spEmp << "\n";
}
