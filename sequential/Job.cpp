#include "Job.h"
#include "ReadDiskDrive.h"
#include "CompressManager.h"
#include "ComputeHash.h"
#include "WritePayload.h"

bool sequential::Job::Capture(
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

bool sequential::Job::DispatchThread(
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

    for (block_id i = 0; i < totalBlocks; i += READ_BLOCK_COUNT) {
        auto limit = i + READ_BLOCK_COUNT;
        limit = min(limit, totalBlocks);

        auto ret = readDiskDrive.Read(i, limit, ReadDiskDrive::compress{});
        if (!ret) { return false; }

        ret = compressMgr.Compress(i, limit);
        if (!ret) { return false; }

        ret = writePayload.Write(i, limit, WritePayload::compress{});
        if (!ret) { return false; }
    }
    printf("\n");

    for (block_id i = 0; i < totalBlocks; i += READ_BLOCK_COUNT) {
        auto limit = i + READ_BLOCK_COUNT;
        limit = min(limit, totalBlocks);

        auto ret = readDiskDrive.Read(i, limit, ReadDiskDrive::calculate_hash{});
        if (!ret) { return false; }

        ret = computeHash.Compute(i, limit);
        if (!ret) { return false; }
    }

    auto ret = writePayload.Write(WritePayload::calculate_hash{});
    if (!ret) { return false; }

    return true;
}

bool sequential::Job::Capture(
    _In_ const wchar_t* drivePath,
    _In_ const wchar_t* imageFile,
    _In_ int            algo,
    _In_ compress,
    _In_ calculate_hash)
{
    diskDrive_ = wil::unique_hfile(::CreateFileW(drivePath, GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!diskDrive_) { return false; }

    if (!DispatchThread(imageFile, algo, compress{}, calculate_hash{})) { return false; }

    return true;
}

bool sequential::Job::DispatchThread(
    _In_ const wchar_t* imageFile,
    _In_ int            algo,
    _In_ compress,
    _In_ calculate_hash)
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

    for (block_id i = 0; i < totalBlocks; i += READ_BLOCK_COUNT) {
        auto limit = i + READ_BLOCK_COUNT;
        limit = min(limit, totalBlocks);

        auto ret = readDiskDrive.Read(i, limit, ReadDiskDrive::compress{}, ReadDiskDrive::calculate_hash{});
        if (!ret) { return false; }

        ret = compressMgr.Compress(i, limit);
        if (!ret) { return false; }

        ret = computeHash.Compute(i, limit);
        if (!ret) { return false; }

        ret = writePayload.Write(i, limit, WritePayload::compress{});
        if (!ret) { return false; }
    }
    printf("\n");

    auto ret = writePayload.Write(WritePayload::calculate_hash{});
    if (!ret) { return false; }

    return true;
}
