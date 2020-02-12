// Lambda.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

class Employee
{
public:
    explicit Employee(std::string id, std::string firstName, std::string lastName) :
        id_(std::move(id)), firstName_(std::move(firstName)), lastName_(std::move(lastName))
    {}

    Employee(const Employee& emp) :
        id_(emp.id_), firstName_(emp.firstName_), lastName_(emp.lastName_)
    {}

    Employee(Employee&& emp) :
        id_(std::move(emp.id_)), firstName_(std::move(emp.firstName_)), lastName_(std::move(emp.lastName_))
    {}

    // not a strong exception safe way
    Employee& operator =(const Employee& emp)
    {
        // avoid self assignment
        if (&emp != this)
        {
            id_ = emp.id_;
            firstName_ = emp.firstName_;
            lastName_ = emp.lastName_;
        }

        return *this;
    }

    Employee& operator =(Employee&& emp) noexcept
    {
        swap(emp);
        return *this;
    }

    void swap(Employee& rhs) noexcept
    {
        using std::swap;
        swap(id_, rhs.id_);
        swap(firstName_, rhs.firstName_);
        swap(lastName_, rhs.lastName_);
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
    os << "Id:" << emp.id_ << " First Name:" << emp.firstName_ << " Last Name:" << emp.lastName_ << "\n";
    return os;
}

class Test
{
public:
    explicit Test(int salary) :
        salary_(salary)
    {}

    void performanceReview()
    {
        // no need for mutable, we didn't change this
        auto adjust = [this]()
        {
            salary_ += 200;
        };
        adjust();
    }

    friend std::ostream& operator <<(std::ostream&, const Test& tst);

private:
    int salary_;
};

std::ostream& operator <<(
    std::ostream& os,
    const Test& tst)
{
    os << "salary:" << tst.salary_ << "\n";
    return os;
}

int main()
{
    std::vector<Employee> employees =
    {
        Employee{ "24688", "J. J.", "Lee" },
        Employee{ "12345", "T. J.", "Wang" },
        Employee{ "11223", "G. H.", "Chin" },
        Employee{ "11220", "C. H.", "Huang" },
    };

    // sorted by Id(), small Id() to big Id()
    std::cout << "\nSorted by Id()\n";
    std::sort(employees.begin(), employees.end(), [](
        const Employee& lhs, const Employee& rhs)
        {
            return lhs.Id().compare(rhs.Id()) < 0;
        });

    std::for_each(employees.begin(), employees.end(), [](
        const Employee& emp)
        {
            std::cout << emp;
        });

    // sorted by FirstName(), small FirstName() to big FirstName()
    std::cout << "\nSorted by FirstName()\n";
    std::sort(employees.begin(), employees.end(), [](
        const Employee& lhs, const Employee& rhs)
        {
            return lhs.FirstName().compare(rhs.FirstName()) < 0;
        });

    std::for_each(employees.begin(), employees.end(), [](
        const Employee& emp)
        {
            std::cout << emp;
        });

    // sorted by LastName(), small LastName() to big LastName()
    std::cout << "\nSorted by LastName()\n";
    std::sort(employees.begin(), employees.end(), [](
        const Employee& lhs, const Employee& rhs)
        {
            return lhs.LastName().compare(rhs.LastName()) < 0;
        });

    std::for_each(employees.begin(), employees.end(), [](
        const Employee& emp)
        {
            std::cout << emp;
        });

    std::cout << "\nPerformance Review\n";
    Test tst(100000);
    tst.performanceReview();
    std::cout << tst;

    return 0;
}
