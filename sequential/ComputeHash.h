#pragma once

#include <vector>
#include <memory>
#include <atomic>
#include "Implement.h"
#include "ReadDiskDrive.h"
#include "..\Crypt.h"

namespace sequential
{

class ComputeHash
{
public:
    using hash_t = Implement::buffer_t;
    using shared_hash_t = Implement::shared_buffer_t;

    using block_id = Implement::block_id;
    using calculate_hash_queue_t = ReadDiskDrive::calculate_hash_queue_t;

    explicit ComputeHash(
        _In_ calculate_hash_queue_t& calculateHashQueue) :
        calculateHashQueue_(calculateHashQueue)
    {
    }

    bool Init(
        _In_ block_id totalBlocks);

    bool Compute(
        _In_ block_id fromBlock,
        _In_ block_id toBlock);

    shared_hash_t GetHash() const;

private:
    calculate_hash_queue_t& calculateHashQueue_;
    bool                    start_ = false;
    Crypt                   sha256_;
    shared_hash_t           hashTable_;
};

}; // namespace sequential
