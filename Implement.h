#pragma once

#include <Windows.h>
#include <memory>
#include <utility>
#include <queue>
#include "Buffer.h"
#include "StateQueue.h"
#include "MonitorQueue.h"
#include "StateVector.h"
#include "MonitorModuloVector.h"

class Implement
{
public:
    // resizeable buffer
    using buffer_t = Buffer<BYTE>;
    using shared_buffer_t = std::shared_ptr<buffer_t>;

    using block_id = DWORD;
    using item = std::pair<block_id, shared_buffer_t>;

    using queue_item = item;
    using state_queue = StateQueue<std::queue<queue_item>>;
    using monitor_queue_t = MonitorQueue<state_queue>;

    using vector_item = shared_buffer_t;
    using state_vector = StateVector<std::vector<vector_item>>;
    using monitor_modulo_vector_t = MonitorModuloVector<state_vector, std::recursive_mutex, std::condition_variable_any>;
};
