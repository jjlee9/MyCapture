// Lambda.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <type_traits>

// Rule of five or maybe six. Apply to the functions I marked as default.
class Employee
{
public:
    /*
    Employee() = default;
    */
    explicit Employee(
        std::string id,
        std::string firstName,
        std::string lastName) noexcept :
        id_(std::move(id)),
        firstName_(std::move(firstName)),
        lastName_(std::move(lastName))
    {
        std::cout << "Employee Id:" << id_ << " Name:" << firstName_ << " "
            << lastName_ << "\n";
    }

    /*
    Employee(
        const Employee& emp) :
        id_(emp.id_),
        firstName_(emp.firstName_),
        lastName_(emp.lastName_)
    {
        std::cout << "Employee(const Employee& emp)\n";
    }
    Employee(
        const Employee& emp) = default;
    */

    /*
    Employee(
        Employee&& emp) noexcept :
        id_(std::move(emp.id_)),
        firstName_(std::move(emp.firstName_)),
        lastName_(std::move(emp.lastName_))
    {
        std::cout << "Employee(Employee&& emp) noexcept\n";
    }
    Employee(
        Employee&& emp) noexcept = default;
    */
    /*
    Employee() = default;
    */

    /*
    // not a strong exception safe way
    Employee& operator =(
        const Employee& rhs)
    {
        std::cout << "Employee& operator =(const Employee& rhs)\n";

        // avoid self assignment
        if (&rhs != this)
        {
            id_ = rhs.id_;
            firstName_ = rhs.firstName_;
            lastName_ = rhs.lastName_;
        }
        return *this;
    }
    Employee& operator =(
        const Employee& rhs) = default;
    */

    /*
    Employee& operator =(
        Employee&& rhs) noexcept
    {
        std::cout << "Employee& operator =(Employee&& rhs) noexcept\n";
        id_ = std::move(rhs.id_);
        firstName_ = std::move(rhs.firstName_);
        lastName_ = std::move(rhs.lastName_);
        return *this;
    }
    Employee& operator =(
        Employee&& rhs) noexcept = default;
    */

    /*
    void swap(Employee& rhs) noexcept
    {
        using std::swap;
        swap(id_, rhs.id_);
        swap(firstName_, rhs.firstName_);
        swap(lastName_, rhs.lastName_);
    }
    */

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

/*
namespace std
{
    template <> void swap<Employee>(Employee& lhs, Employee& rhs)
    {
        lhs.swap(rhs);
    }
}
*/

std::ostream& operator <<(
    std::ostream& os,
    const Employee& emp)
{
    // setw for alignment
    os << "Id:" << std::setw(5)
        << emp.id_ << " Name:" << std::setw(5) << emp.firstName_ << " "
        << std::left << std::setw(5) << emp.lastName_
        <<"\n";
    return os;
}

// for demo purpose only, not a good design
void performaceReview(
    int& monthlySalary,
    int& bonus)
{
    // monthlySalary change is for the next year
    monthlySalary = static_cast<std::remove_reference_t<decltype(monthlySalary)>>(monthlySalary * 1.1);
    // bonus chnage is for this year
    bonus = 60000;
}

int main()
{
    std::cout << "Employ vector\n";
    std::vector<Employee> employees =
    {
        Employee{ "13579", "J. J.", "Lee"   },
        Employee{ "12345", "T. J.", "Wang"  },
        Employee{ "11223", "G. H.", "Chin"  },
        Employee{ "11220", "C. H.", "Huang" },
    };
    std::cout << "\n";

    // sorted by Id()
    std::cout << "Sorted by Id()\n";
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

    // sorted by FirstName()
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

    // sorted by LastName()
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
    int monthlySalary = 50000;
    int bonus = monthlySalary * 1;

    auto yearlyIncome = [monthlySalary, &bonus]
    {
        return monthlySalary * 12 + bonus;
    };

    performaceReview(monthlySalary, bonus);
    // Hey, please calculate my income this year!
    // == 50000 * 12 + 60000 == 66000
    int myIncomeThisYear = yearlyIncome();

    std::cout << "monthlySalary:" << monthlySalary
        << " bonus:" << bonus << " myIncomeThisYear:" << myIncomeThisYear << "\n";

    return 0;
}
