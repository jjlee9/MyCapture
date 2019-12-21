#pragma once

class WorkThreads
{
public:
    static constexpr int READ_DRIVE_THREAD_START = 0;
    static constexpr int READ_DRIVE_THREAD_COUNT = 1;
    static constexpr int COMPRESS_THREAD_START = READ_DRIVE_THREAD_START + READ_DRIVE_THREAD_COUNT;
    static constexpr int COMPRESS_THREAD_COUNT = 4;
    static constexpr int CALCULATE_HASH_THREAD_START = COMPRESS_THREAD_START + COMPRESS_THREAD_COUNT;
    static constexpr int CALCULATE_HASH_THREAD_COUNT = 4;
    static constexpr int WRITE_PAYLOAD_THREAD_START = CALCULATE_HASH_THREAD_START + CALCULATE_HASH_THREAD_COUNT;
    static constexpr int WRITE_PAYLOAD_THREAD_COUNT = 1;
    static constexpr int TOTAL_THREAD_COUNT = READ_DRIVE_THREAD_COUNT + COMPRESS_THREAD_COUNT + CALCULATE_HASH_THREAD_COUNT + WRITE_PAYLOAD_THREAD_COUNT;
};
