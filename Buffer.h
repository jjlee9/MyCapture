#pragma once

#include <assert.h>
#include <vector>

template <typename T> class Buffer
{
public:
    explicit Buffer(
        _In_ LONGLONG size = 0)
    {
        static_assert(std::is_trivially_constructible_v<T> == true, "Not a trivially constructible type");
        Resize(size);
    }

    void Resize(
        _In_ LONGLONG size)
    {
        buffer_.resize(size);
    }

    size_t Size() const { return buffer_.size(); }

    T* Data() { return buffer_.data(); }
    const T* Data() const { return buffer_.data(); }

protected:
    std::vector<T> buffer_;
};
