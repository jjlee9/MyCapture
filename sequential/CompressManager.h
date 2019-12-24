#pragma once

#include <Windows.h>
#include "Implement.h"
#include "ReadDiskDrive.h"
#include "..\CompressDll.h"

namespace sequential
{

class CompressManager
{
public:
    using buffer_t = Implement::buffer_t;
    using shared_buffer_t = Implement::shared_buffer_t;

    using block_id = Implement::block_id;
    using queue_item = Implement::queue_item;

    using uncompressed_queue_t = ReadDiskDrive::uncompressed_queue_t;
    using compressed_vector_t = Implement::vector_t;

    static constexpr auto WRITE_BLOCK_COUNT = DiskConsts::WRITE_BLOCK_COUNT;

    explicit CompressManager(
        _In_ int                   algo,
        _In_ uncompressed_queue_t& uncompressedQueue);

    bool Init(
        _In_ block_id totalBlocks);

    bool Compress(
        _In_ block_id fromBlock,
        _In_ block_id toBlock);

    compressed_vector_t& CompressedVector() { return compressedVector_; }

private:
    shared_buffer_t CompressBlock(
        _In_ block_id          blockNo,
        _In_ shared_buffer_t&& uncompressBlock);

    CompressDll           compressDll_;
    int                   algo_;                       // compress algorithm
    uncompressed_queue_t& unompressedQueue_;           // read uncompressed data for compression
    compressed_vector_t   compressedVector_;           // compressed data
};

}; // namespace sequential
