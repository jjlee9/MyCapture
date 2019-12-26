#include "CompressManager.h"

sequential::CompressManager::CompressManager(
    _In_ int                   algo,
    _In_ uncompressed_queue_t& uncompressedQueue) :
    algo_(algo),
    unompressedQueue_(uncompressedQueue)
{
}

bool sequential::CompressManager::Init(
    _In_ block_id totalBlocks)
{
    auto ok = compressDll_.Init();
    if (!ok) { return ok; }

    compressedVector_.resize(WRITE_BLOCK_COUNT);

    return true;
}

bool sequential::CompressManager::Compress(
    _In_ block_id fromBlock,
    _In_ block_id toBlock)
{
    auto& que = unompressedQueue_;

    for (auto i = fromBlock; i < toBlock; i++) {
        auto item = que.front();
        que.pop();

        auto compressBlock = CompressBlock(item.first, std::move(item.second));
        if (!compressBlock) {
            return false;
        }

        compressedVector_[item.first % compressedVector_.size()] = compressBlock;
    }

    return true;
}

sequential::CompressManager::shared_buffer_t sequential::CompressManager::CompressBlock(
    _In_ block_id          blockNo,
    _In_ shared_buffer_t&& uncompressBlock)
{
    auto compressBlock = std::make_shared<buffer_t>(uncompressBlock->size() + 4 * 1024);

    std::unique_ptr<BYTE[]> workSpace;
    ULONG workSpaceSize = 0, fragmentWorkSpaceSize = 0;
    auto ret = (*compressDll_.rtlGetCompressionWorkspaceSize)(
        algo_ | COMPRESSION_ENGINE_STANDARD, &workSpaceSize, &fragmentWorkSpaceSize);
    if (ret == 0) {
        workSpace = std::make_unique<BYTE[]>(workSpaceSize);
    }
    if (!workSpace) { return shared_buffer_t(); } // empty buffer

    ULONG compressSize = 0;
    ret = (*compressDll_.rtlCompressBuffer)(
        algo_ | COMPRESSION_ENGINE_STANDARD,
        uncompressBlock->data(), static_cast<ULONG>(uncompressBlock->size()),
        compressBlock->data(), static_cast<ULONG>(compressBlock->size()),
        4 * 1024, &compressSize, workSpace.get());

    DWORD lastError;
    if (ret == 0xC0000023) {
        lastError = ERROR_INSUFFICIENT_BUFFER;
    } else {
        lastError = ::GetLastError();
    }

    if (lastError == ERROR_INSUFFICIENT_BUFFER) {
        compressBlock->resize(compressSize);
        ret = (*compressDll_.rtlCompressBuffer)(
            algo_ | COMPRESSION_ENGINE_MAXIMUM,
            uncompressBlock->data(), static_cast<ULONG>(uncompressBlock->size()),
            compressBlock->data(), static_cast<ULONG>(compressBlock->size()),
            4 * 1024, &compressSize, workSpace.get());
    }

    if (ret == 0) {
        compressBlock->resize(compressSize);
        return compressBlock;
    } else {
        return shared_buffer_t(); // empty buffer
    }
}

