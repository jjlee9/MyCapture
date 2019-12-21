#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "WorkThreads.h"
#include "Implement.h"
#include "ReadDiskDrive.h"
#include "Crypt.h"
#include "Buffer.h"

class ComputeHash
{
public:
    using hash_t = Implement::buffer_t;
    using shared_hash_t = Implement::shared_buffer_t;
    using hash_data_t = BYTE*;

    using block_id = Implement::block_id;
    using calculate_hash_queue_t = ReadDiskDrive::calculate_hash_queue_t;

    static constexpr auto CALCULATE_HASH_THREAD_COUNT = WorkThreads::CALCULATE_HASH_THREAD_COUNT;

    explicit ComputeHash(
        _In_ calculate_hash_queue_t& calculateHashQueue) :
        calculateHashQueue_(calculateHashQueue)
    {
    }

    bool Init(
        _In_ block_id totalBlocks);

    void Start();

    void Stop();

    DWORD Compute();

    shared_hash_t WaitHash();

private:
    using mutex_t = std::mutex;
    using cond_t = std::condition_variable;

    shared_hash_t GetHash() const;

    DWORD Return(DWORD ret) const
    {
        if (start_) {
            return ret;
        } else {
            // the functionality is stopped intentionally, don't report any error
            return 0;
        }
    }

    calculate_hash_queue_t& calculateHashQueue_;
    bool                    start_ = false;
    Crypt                   sha256_;
    shared_hash_t           hashTable_;

    std::atomic<int>        completeThreads_ = 0;
    mutable mutex_t         mutex_;
    cond_t                  hashFinished_; // hash is complete or failed
    std::vector<bool>       hashDone_;     // hash is done or not for each blocks
};
