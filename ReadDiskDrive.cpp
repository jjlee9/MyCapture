#include "ReadDiskDrive.h"

bool ReadDiskDrive::Init()
{
    Start();

    return true;
}

void ReadDiskDrive::Start()
{
    start_ = true;
    calculateHashQueue_.Start();
    uncompressedQueue_.Start();
    {
        std::unique_lock<mutex_t> lock(mutex_);
        limitBlock_ = 0;
    }
}

void ReadDiskDrive::Stop()
{
    start_ = false;
    uncompressedQueue_.Stop();
    calculateHashQueue_.Stop();
    {
        std::unique_lock<mutex_t> lock(mutex_);
        queueItemCount_ = 0;
    }
    queueItemConsumed_.notify_all();
}

DWORD ReadDiskDrive::Read(
    _In_ block_id totalBlocks)
{
    try {
        static_assert(READ_DRIVE_THREAD_COUNT == 1, "READ_DRIVE_THREAD_COUNT should be 1");
        for (decltype(totalBlocks) i = 0; i < totalBlocks; ++i) {
            TriggerRead(i);
        }
        End();
        return Return(0);
    } catch (monitor_queue_t::Failure&) {
        return Return(10);
    } catch (...) {
        return Return(11);
    }
}

bool ReadDiskDrive::TriggerRead(
    _In_ block_id blockId)
{
    if (blockId >  limitBlock_) {
        for (;;) {
            std::unique_lock<mutex_t> lock(mutex_);
            if (queueItemCount_ <= TRIGGER_READ_BLOCK_COUNT) { break; }
            queueItemConsumed_.wait(lock);
        }

        // read limitBlock_ READ_BLOCK_COUNT blocks ahead
        limitBlock_ += READ_BLOCK_COUNT;
    }

    // shared memory buffer with a custom deleter
    shared_buffer_t block(new buffer_t(static_cast<LONGLONG>(BLOCK_LENGTH)), [this](buffer_t* buffer)
        {
            bool notify;
            {
                std::lock_guard<mutex_t> lock(mutex_);
                auto count = --queueItemCount_;
                delete buffer;
                static_assert(TRIGGER_READ_BLOCK_COUNT < READ_BLOCK_COUNT, "TRIGGER_READ_BLOCK_COUNT must less than READ_BLOCK_COUNT");
                notify = count == TRIGGER_READ_BLOCK_COUNT;
            }
            if (notify) { queueItemConsumed_.notify_one(); }
        });

    LARGE_INTEGER offset;
    offset.QuadPart = static_cast<decltype(LARGE_INTEGER::QuadPart)>(blockId) * BLOCK_LENGTH;
    // clear the buffer for security read block from drive, if needed
    // memset(block->Data(), 0, block->Size());

    ::SetFilePointerEx(diskDrive_.get(), offset, nullptr, FILE_BEGIN);
    DWORD size = 0;
    auto ok = ReadFile(diskDrive_.get(), block->Data(), static_cast<DWORD>(block->Size()), &size, 0);
    if (ok) {
        if (size != BLOCK_LENGTH) {
            block->Resize(size);
        }
        Push(blockId, std::move(block));
    }

    return ok != FALSE;
}

void ReadDiskDrive::Push(
    _In_ block_id          blockId,
    _In_ shared_buffer_t&& diskBlock)
{
    std::lock_guard<mutex_t> lock(mutex_);
    calculateHashQueue_.push(std::make_pair(blockId, diskBlock));
    uncompressedQueue_.push(std::make_pair(blockId, std::move(diskBlock)));
    ++queueItemCount_;
}

void ReadDiskDrive::End()
{
    // push an empty shared buffer - sentinel for end condition
    for (std::remove_const_t<decltype(COMPRESS_THREAD_COUNT)> i = 0; i < COMPRESS_THREAD_COUNT; ++i) {
        uncompressedQueue_.push(std::make_pair(0, shared_buffer_t()));
    }

    for (std::remove_const_t<decltype(CALCULATE_HASH_THREAD_COUNT)> i = 0; i < CALCULATE_HASH_THREAD_COUNT; ++i) {
        calculateHashQueue_.push(std::make_pair(0, shared_buffer_t()));
    }
}
