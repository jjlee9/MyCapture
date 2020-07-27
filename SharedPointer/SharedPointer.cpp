#include <iostream>
#include <vector>
#include <memory>
#include <thread>

using byte = unsigned char;

void f(byte* /*buffer*/)
{
}

class X
{
public:
    static constexpr int BUFFER_SIZE = 1000;

    X() :
        m_shared_ptr(std::make_shared<byte>(BUFFER_SIZE))
    {
    }

    void method()
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
    std::shared_ptr<byte> m_shared_ptr;
};

int main()
{
    X x;

    x.method();
}
