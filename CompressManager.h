#pragma once

#include <Windows.h>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "WorkThreads.h"
#include "DiskConsts.h"
#include "Implement.h"
#include "ReadDiskDrive.h"
#include "CompressDll.h"

class CompressManager
{
public:
    using buffer_t = Implement::buffer_t;
    using shared_buffer_t = Implement::shared_buffer_t;

    using block_id = Implement::block_id;
    using queue_item = Implement::queue_item;

    using uncompressed_queue_t = ReadDiskDrive::uncompressed_queue_t;
    using compressed_vector_t = Implement::monitor_modulo_vector_t;

    static constexpr auto COMPRESS_THREAD_COUNT = WorkThreads::COMPRESS_THREAD_COUNT;
    static constexpr auto WRITE_PAYLOAD_THREAD_COUNT = WorkThreads::WRITE_PAYLOAD_THREAD_COUNT;

    static constexpr auto WRITE_BLOCK_COUNT = DiskConsts::WRITE_BLOCK_COUNT;

    explicit CompressManager(
        _In_ int                   algo,
        _In_ uncompressed_queue_t& uncompressedQueue);

    bool Init(
        _In_ block_id totalBlocks);

    void Start();

    void Stop();

    DWORD Compress();

    compressed_vector_t& CompressedVector() { return compressedVector_; }

    void WaitCompress();

private:
    using mutex_t = std::mutex;
    using cond_t = std::condition_variable;

    shared_buffer_t CompressBlock(
        _In_ block_id          blockNo,
        _In_ shared_buffer_t&& uncompressBlock);

    DWORD Return(DWORD ret) const
    {
        if (start_) {
            return ret;
        } else {
            // the functionality is stopped intentionally, don't report any error
            return 0;
        }
    }

    CompressDll           compressDll_;
    int                   algo_;                       // compress algorithm
    bool                  start_ = false;
    uncompressed_queue_t& unompressedQueue_;           // read uncompressed data for compression
    compressed_vector_t   compressedVector_;           // compressed data

    std::atomic<int>      completeThreads_ = 0;
    mutable mutex_t       mutex_;
    bool                  succeeded_ = true;
    cond_t                compressedContainerChanged_;
};
