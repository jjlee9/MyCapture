#include "ReadDiskDrive.h"

bool sequential::ReadDiskDrive::Init()
{
    return true;
}

bool sequential::ReadDiskDrive::Read(
    _In_ block_id fromBlock,
    _In_ block_id toBlock,
    _In_ compress)
{
    LARGE_INTEGER offset;
    offset.QuadPart = static_cast<decltype(LARGE_INTEGER::QuadPart)>(fromBlock) * BLOCK_LENGTH;
    ::SetFilePointerEx(diskDrive_.get(), offset, nullptr, FILE_BEGIN);

    for (auto i = fromBlock; i < toBlock; ++i) {
        auto block = std::make_shared<buffer_t>(static_cast<LONGLONG>(BLOCK_LENGTH));
        // clear the buffer for security read block from drive, if needed
        // memset(block->Data(), 0, block->Size());

        DWORD size = 0;
        auto ok = ::ReadFile(diskDrive_.get(), block->Data(), static_cast<DWORD>(block->Size()), &size, 0);
        if (ok == FALSE) { return false; }

        if (size != BLOCK_LENGTH) {
            block->Resize(size);
        }
        uncompressedQueue_.emplace(i, std::move(block));
    }

    return true;
}

bool sequential::ReadDiskDrive::Read(
    _In_ block_id       fromBlock,
    _In_ block_id       toBlock,
    _In_ calculate_hash)
{
    LARGE_INTEGER offset;
    offset.QuadPart = static_cast<decltype(LARGE_INTEGER::QuadPart)>(fromBlock) * BLOCK_LENGTH;
    ::SetFilePointerEx(diskDrive_.get(), offset, nullptr, FILE_BEGIN);

    for (auto i = fromBlock; i < toBlock; ++i) {
        auto block = std::make_shared<buffer_t>(static_cast<LONGLONG>(BLOCK_LENGTH));
        // clear the buffer for security read block from drive, if needed
        // memset(block->Data(), 0, block->Size());

        DWORD size = 0;
        auto ok = ::ReadFile(diskDrive_.get(), block->Data(), static_cast<DWORD>(block->Size()), &size, 0);
        if (ok == FALSE) { return false; }

        if (size != BLOCK_LENGTH) {
            block->Resize(size);
        }
        calculateHashQueue_.emplace(i, std::move(block));
    }

    return true;
}

bool sequential::ReadDiskDrive::Read(
    _In_ block_id       fromBlock,
    _In_ block_id       toBlock,
    _In_ compress,
    _In_ calculate_hash)
{
    LARGE_INTEGER offset;
    offset.QuadPart = static_cast<decltype(LARGE_INTEGER::QuadPart)>(fromBlock) * BLOCK_LENGTH;
    ::SetFilePointerEx(diskDrive_.get(), offset, nullptr, FILE_BEGIN);

    for (auto i = fromBlock; i < toBlock; ++i) {
        auto block = std::make_shared<buffer_t>(static_cast<LONGLONG>(BLOCK_LENGTH));
        // clear the buffer for security read block from drive, if needed
        // memset(block->Data(), 0, block->Size());

        DWORD size = 0;
        auto ok = ::ReadFile(diskDrive_.get(), block->Data(), static_cast<DWORD>(block->Size()), &size, 0);
        if (ok == FALSE) { return false; }

        if (size != BLOCK_LENGTH) {
            block->Resize(size);
        }
        uncompressedQueue_.emplace(i, block);
        calculateHashQueue_.emplace(i, std::move(block));
    }

    return true;
}
