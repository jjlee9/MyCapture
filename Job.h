#pragma once

#include <Windows.h>
#include <wil/resource.h>
#include "WorkThreads.h"
#include "DiskConsts.h"
#include "Implement.h"

struct ThreadParam;

class Job
{
public:
    using block_id = Implement::block_id;

    static constexpr auto READ_DRIVE_THREAD_START = WorkThreads::READ_DRIVE_THREAD_START;
    static constexpr auto READ_DRIVE_THREAD_COUNT = WorkThreads::READ_DRIVE_THREAD_COUNT;
    static constexpr auto COMPRESS_THREAD_START = WorkThreads::COMPRESS_THREAD_START;
    static constexpr auto COMPRESS_THREAD_COUNT = WorkThreads::COMPRESS_THREAD_COUNT;
    static constexpr auto CALCULATE_HASH_THREAD_START = WorkThreads::CALCULATE_HASH_THREAD_START;
    static constexpr auto CALCULATE_HASH_THREAD_COUNT = WorkThreads::CALCULATE_HASH_THREAD_COUNT;
    static constexpr auto WRITE_PAYLOAD_THREAD_START = WorkThreads::WRITE_PAYLOAD_THREAD_START;
    static constexpr auto WRITE_PAYLOAD_THREAD_COUNT = WorkThreads::WRITE_PAYLOAD_THREAD_COUNT;
    static constexpr auto TOTAL_THREAD_COUNT = WorkThreads::TOTAL_THREAD_COUNT;

    static constexpr auto BLOCK_LENGTH = DiskConsts::BLOCK_LENGTH;

    bool Capture(
        _In_ const wchar_t* drivePath,
        _In_ const wchar_t* imageFile,
        _In_ int            algo);

private:
    bool DispatchThread(
        _In_ const wchar_t* imageFile,
        _In_ int            algo);

    bool ReadDrive(
        _In_ ThreadParam&);
    static DWORD WINAPI ReadDriveThreadFunction(
        _In_ LPVOID param);

    bool Compress(
        _In_ ThreadParam&);
    static DWORD WINAPI CompressThreadFunction(
        _In_ LPVOID param);

    bool CalculateHash(
        _In_ ThreadParam&);
    static DWORD WINAPI CalculateHashThreadFunction(
        _In_ LPVOID param);

    bool WriteResult(
        _In_ ThreadParam&);
    static DWORD WINAPI WriteResultThreadFunction(
        _In_ LPVOID param);

    wil::unique_hfile diskDrive_;
};
