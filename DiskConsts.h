#pragma once

#include <windef.h>
#include "WorkThreads.h"

class DiskConsts
{
public:
    static constexpr DWORD BLOCK_LENGTH = 128 * 1024; // 128KB

    // We hope to read in up to READ_BLOCK_COUNT blocks in one trigger, it is triggered on TRIGGER_BLOCK_COUNT.
    static constexpr auto READ_BLOCK_COUNT = WorkThreads::CALCULATE_HASH_THREAD_COUNT * 32; // high water
    static constexpr auto TRIGGER_READ_BLOCK_COUNT = WorkThreads::CALCULATE_HASH_THREAD_COUNT * 2; // low water

    // We hope to write out up to WRITE_BLOCK_COUNT blocks in one trigger
    static constexpr auto WRITE_BLOCK_COUNT = READ_BLOCK_COUNT;
};
