#include "Job.h"
#include <memory>
#include "ThreadParam.h"
#include "WorkThreads.h"
#include "MonitorQueue.h"
#include "ReadDiskDrive.h"
#include "ComputeHash.h"
#include "CompressManager.h"
#include "WritePayload.h"

bool Job::Capture(
    _In_ const wchar_t* drivePath,
    _In_ const wchar_t* imageFile,
    _In_ int            algo)
{
    diskDrive_ = wil::unique_hfile(::CreateFileW(drivePath, GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!diskDrive_) { return false; }

    if (!DispatchThread(imageFile, algo)) { return false; }

    return true;
}

bool Job::DispatchThread(
    _In_ const wchar_t* imageFile,
    _In_ int            algo)
{
    LARGE_INTEGER fileSize;
    if (!::GetFileSizeEx(diskDrive_.get(), &fileSize)) { return false; }

    auto totalBlocks = static_cast<block_id>((fileSize.QuadPart + BLOCK_LENGTH + 1) / BLOCK_LENGTH);
    ReadDiskDrive readDiskDrive(diskDrive_);
    if (!readDiskDrive.Init()) { return false; }
    CompressManager compressMgr(algo, readDiskDrive.UncompressedQueue());
    if (!compressMgr.Init(totalBlocks)) { return false; }
    ComputeHash computeHash(readDiskDrive.CalculateHashQueue());
    if (!computeHash.Init(totalBlocks)) { return false; }
    WritePayload writePayload(compressMgr, computeHash);
    if (!writePayload.Init(imageFile)) { return false; }

    ThreadParam thrdParam { totalBlocks, readDiskDrive, compressMgr, computeHash, writePayload };
    thrdParam.threads.resize(TOTAL_THREAD_COUNT);
    auto hThreads = std::make_unique<HANDLE[]>(TOTAL_THREAD_COUNT);
    bool waited = false;

    auto exit = wil::scope_exit([&thrdParam, &hThreads, &waited]
        {
            // We already ::WaitForMultipleObjects successfully, no need to wait again!
            if (waited) { return; }

            // for ReadDrive, CalculateHash, Compress return false or throw an exception
            thrdParam.readDiskDrive.Stop();
            std::remove_const_t<decltype(TOTAL_THREAD_COUNT)> nthreads = 0;
            for (std::remove_const_t<decltype(TOTAL_THREAD_COUNT)> i = 0; i < TOTAL_THREAD_COUNT; ++i) {
                if (!thrdParam.threads[i]) { continue; }
                hThreads.get()[nthreads++] = thrdParam.threads[i].get();
            }
            ::WaitForMultipleObjects(nthreads, hThreads.get(), TRUE, INFINITE);
        });

    if (!ReadDrive(thrdParam)) { return false; }
    if (!Compress(thrdParam)) { return false; }
    if (!CalculateHash(thrdParam)) { return false; }
    if (!WriteResult(thrdParam)) { return false; }

    // wait for all threads
    for (std::remove_const_t<decltype(TOTAL_THREAD_COUNT)> i = 0; i < TOTAL_THREAD_COUNT; ++i) {
        hThreads.get()[i] = thrdParam.threads[i].get();
    }
    ::WaitForMultipleObjects(TOTAL_THREAD_COUNT, hThreads.get(), TRUE, INFINITE);
    waited = true;

    return true;
}

bool Job::ReadDrive(
    _In_ ThreadParam& thrdParam)
{
    thrdParam.threads[READ_DRIVE_THREAD_START] = ThreadParam::unique_thread_handle(
        ::CreateThread(nullptr, 0, ReadDriveThreadFunction, &thrdParam, 0, nullptr));
    auto& thrd = thrdParam.threads[READ_DRIVE_THREAD_START];
    if (!thrd) { return false; }

    return true;
}

DWORD WINAPI Job::ReadDriveThreadFunction(
    _In_ LPVOID param)
{
    auto p = static_cast<ThreadParam*>(param);
    auto& readDiskDrive = p->readDiskDrive;
    auto ret = readDiskDrive.Read(p->totalBlocks);
    if (ret != 0) {
        printf("\nerror %d\n", ret);
    }
    return ret;
}

bool Job::Compress(
    _In_ ThreadParam& thrdParam)
{
    for (std::remove_const_t<decltype(COMPRESS_THREAD_COUNT)> i = 0; i < COMPRESS_THREAD_COUNT; ++i) {
        thrdParam.threads[COMPRESS_THREAD_START + i] = ThreadParam::unique_thread_handle(
            ::CreateThread(nullptr, 0, CompressThreadFunction, &thrdParam, 0, nullptr));
        auto& thrd = thrdParam.threads[COMPRESS_THREAD_START + i];
        if (!thrd) { return false; }
    }

    return true;
}

DWORD WINAPI Job::CompressThreadFunction(
    _In_ LPVOID param)
{
    auto p = static_cast<ThreadParam*>(param);
    auto& compressMgr = p->compressMgr;
    auto ret = compressMgr.Compress();
    if (ret != 0) {
        printf("\nerror %d\n", ret);
    }
    return ret;
}

bool Job::CalculateHash(
    _In_ ThreadParam& thrdParam)
{
    for (std::remove_const_t<decltype(CALCULATE_HASH_THREAD_COUNT)> i = 0; i < CALCULATE_HASH_THREAD_COUNT; ++i) {
        thrdParam.threads[CALCULATE_HASH_THREAD_START + i] = ThreadParam::unique_thread_handle(
            ::CreateThread(nullptr, 0, CalculateHashThreadFunction, &thrdParam, 0, nullptr));
        auto& thrd = thrdParam.threads[CALCULATE_HASH_THREAD_START + i];
        if (!thrd) { return false; }
    }

    return true;
}

DWORD WINAPI Job::CalculateHashThreadFunction(
    _In_ LPVOID param)
{
    auto p = static_cast<ThreadParam*>(param);
    auto& computeHash = p->computeHash;
    auto ret = computeHash.Compute();
    if (ret != 0) {
        printf("\nerror %d\n", ret);
    }
    return ret;
}

bool Job::WriteResult(
    _In_ ThreadParam& thrdParam)
{
    thrdParam.threads[WRITE_PAYLOAD_THREAD_START] = ThreadParam::unique_thread_handle(
        ::CreateThread(nullptr, 0, WriteResultThreadFunction, &thrdParam, 0, nullptr));
    auto& thrd = thrdParam.threads[WRITE_PAYLOAD_THREAD_START];
    if (!thrd) { return false; }

    return true;
}

DWORD WINAPI Job::WriteResultThreadFunction(
    _In_ LPVOID param)
{
    auto p = static_cast<ThreadParam*>(param);
    auto& writePayload = p->writePayload;
    auto ret = writePayload.Write(p->totalBlocks);
    if (ret != 0) {
        printf("\nerror %d\n", ret);
    }
    return ret;
}
