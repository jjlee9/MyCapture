#include <iostream>
#include <vector>
#include <memory>
#include <thread>

using byte = unsigned char;

void f(const std::vector<byte>& /*buffer*/)
{
}

template <typename T>
class MyAlloc : public std::allocator<T>
{
    using base = std::allocator<T>;
    using base::base;

public:
};

class X
{
public:
    static constexpr int BUFFER_SIZE = 1000;

    X() :
        m_shared_ptr(std::allocate_shared<std::vector<byte>, std::allocator>(BUFFER_SIZE))
    {
    }

    ~X()
    {
    }

    void fun()
    {
        auto sptr = m_shared_ptr; // sptr responsible for dispatch to diff threads
        std::vector<std::thread> threads;

        for (int i = 0; i < 4; ++i)
        {
            threads.push_back(std::thread([sptr]() // copied to 4 threads
                {
                    f(*sptr.get());
                }));
        }

        for (int i = 0; i < 4; ++i)
        {
            threads[i].join();
        }
    }

private:
    std::shared_ptr<std::vector<byte>> m_shared_ptr;
};

int main()
{
    X x;

    x.fun();
}
