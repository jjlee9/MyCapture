#pragma once

#include <Windows.h>
#include <wil/resource.h>
#include <memory>
#include "DiskConsts.h"
#include "Implement.h"

namespace sequential
{

class ReadDiskDrive
{
public:
    using buffer_t = Implement::buffer_t;
    using shared_buffer_t = Implement::shared_buffer_t;

    using block_id = Implement::block_id;
    using queue_item = Implement::queue_item;

    using queue_t = Implement::queue_t;
    using calculate_hash_queue_t = queue_t;
    using uncompressed_queue_t = queue_t;

    static constexpr auto BLOCK_LENGTH = DiskConsts::BLOCK_LENGTH;

    static constexpr auto READ_BLOCK_COUNT = DiskConsts::READ_BLOCK_COUNT;

    explicit ReadDiskDrive(
        _In_ const wil::unique_hfile& diskDrive) :
        diskDrive_(diskDrive)
    {
    }

    bool Init();

    struct compress {};

    bool Read(
        _In_ block_id fromBlock,
        _In_ block_id toBlock,
        _In_ compress);

    struct calculate_hash {};

    bool Read(
        _In_ block_id       fromBlock,
        _In_ block_id       toBlock,
        _In_ calculate_hash);

    bool Read(
        _In_ block_id       fromBlock,
        _In_ block_id       toBlock,
        _In_ compress,
        _In_ calculate_hash);

    uncompressed_queue_t& UncompressedQueue() { return uncompressedQueue_; }
    calculate_hash_queue_t& CalculateHashQueue() { return calculateHashQueue_; }

private:
    const wil::unique_hfile& diskDrive_;

    uncompressed_queue_t     uncompressedQueue_;   // queue of uncompressed disk block memory buffer for compression
    calculate_hash_queue_t   calculateHashQueue_;  // queue of disk block memory buffer for calculating hash
};

}; // namespace sequential
