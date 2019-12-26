#pragma once

#include <Windows.h>
#include <vector>
#include <memory>
#include <utility>
#include <queue>

namespace sequential
{

class Implement
{
public:
    // resizeable buffer
    using buffer_t = std::vector<BYTE>;
    using shared_buffer_t = std::shared_ptr<buffer_t>;

    using block_id = DWORD;
    using item = std::pair<block_id, shared_buffer_t>;

    using queue_item = item;
    using queue_t = std::queue<queue_item>;

    using vector_item = shared_buffer_t;
    using vector_t = std::vector<vector_item>;
};

}; // namespace sequential
