#pragma once

#include <Windows.h>
#include <wil/resource.h>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "WorkThreads.h"
#include "DiskConsts.h"
#include "Implement.h"

class ReadDiskDrive
{
public:
    using buffer_t = Implement::buffer_t;
    using shared_buffer_t = Implement::shared_buffer_t;

    using block_id = Implement::block_id;
    using queue_item = Implement::queue_item;

    using state_queue = Implement::state_queue;
    using monitor_queue_t = Implement::monitor_queue_t;
    using calculate_hash_queue_t = monitor_queue_t;
    using uncompressed_queue_t = monitor_queue_t;

    static constexpr auto READ_DRIVE_THREAD_COUNT = WorkThreads::READ_DRIVE_THREAD_COUNT;
    static constexpr auto COMPRESS_THREAD_COUNT = WorkThreads::COMPRESS_THREAD_COUNT;
    static constexpr auto CALCULATE_HASH_THREAD_COUNT = WorkThreads::CALCULATE_HASH_THREAD_COUNT;

    static constexpr auto BLOCK_LENGTH = DiskConsts::BLOCK_LENGTH;

    static constexpr auto READ_BLOCK_COUNT = DiskConsts::READ_BLOCK_COUNT;
    static constexpr auto TRIGGER_READ_BLOCK_COUNT = DiskConsts::TRIGGER_READ_BLOCK_COUNT;

    explicit ReadDiskDrive(
        _In_ const wil::unique_hfile& diskDrive) :
        diskDrive_(diskDrive),
        queueItemCount_(0)
    {
    }

    bool Init();

    void Start();

    void Stop();

    DWORD Read(
        _In_ block_id totalBlocks);

    calculate_hash_queue_t& CalculateHashQueue() { return calculateHashQueue_; }
    uncompressed_queue_t& UncompressedQueue() { return uncompressedQueue_; }

private:
    using mutex_t = std::mutex;
    using cond_t = std::condition_variable;

    bool TriggerRead(
        _In_ block_id blockId);

    // shared memory buffer for nth disk block
    void Push(
        _In_ block_id          blockId,
        _In_ shared_buffer_t&& diskBlock);

    void End();

    DWORD Return(DWORD ret) const
    {
        if (start_) {
            return ret;
        } else {
            // the functionality is stopped intentionally, don't report any error
            return 0;
        }
    }

    const wil::unique_hfile& diskDrive_;
    bool                     start_ = false;

    mutable mutex_t          mutex_;
    cond_t                   queueItemConsumed_;
    DWORD                    queueItemCount_ = 0;  // min(calculateHashQueue_, compressQueue_)
    DWORD                    limitBlock_ = 0;      // limit to read up to this block

    calculate_hash_queue_t   calculateHashQueue_;  // queue of disk block memory buffer for calculating hash
    uncompressed_queue_t     uncompressedQueue_;   // queue of uncompressed disk block memory buffer for compression
};