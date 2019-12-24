#pragma once
#include <windef.h>

namespace sequential
{

class DiskConsts
{
public:
    static constexpr DWORD BLOCK_LENGTH = 128 * 1024; // 128KB

    static constexpr auto READ_BLOCK_COUNT = 4 * 32;

    static constexpr auto WRITE_BLOCK_COUNT = READ_BLOCK_COUNT;
};

}; // namespace sequential
