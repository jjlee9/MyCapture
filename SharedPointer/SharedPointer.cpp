#include <iostream>
#include <vector>
#include <memory>
#include <thread>

using BYTE = unsigned char;

void f(BYTE buffer[])
{
}

class X
{
public:
    static constexpr int HD_BLOCK_SIZE = 4096;

    X() :
        m_shared_ptr(std::make_shared<BYTE[]>(HD_BLOCK_SIZE))
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
                    f(sptr.get());
                }));
        }

        for (int i = 0; i < 4; ++i)
        {
            threads[i].join();
        }
    }

private:
    std::shared_ptr<BYTE[]> m_shared_ptr;
};

int main()
{
    X x;

    x.fun();
}
