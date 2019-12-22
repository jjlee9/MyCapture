#include "CompressManager.h"
#include <wil/resource.h>
#include <thread>
#include <chrono>

CompressManager::CompressManager(
    _In_ int                   algo,
    _In_ uncompressed_queue_t& uncompressedQueue) :
    algo_(algo),
    unompressedQueue_(uncompressedQueue)
{
}

bool CompressManager::Init(
    _In_ block_id totalBlocks)
{
    auto ok = compressDll_.Init();
    if (!ok) { return ok; }

    ok = compressedVector_.Init(WRITE_BLOCK_COUNT, totalBlocks);
    if (!ok) { return ok; }

    completeThreads_ = 0;
    succeeded_ = true;

    Start();

    return true;
}

void CompressManager::Start()
{
    start_ = true;
    unompressedQueue_.Start();
    compressedVector_.Start();
}

void CompressManager::Stop()
{
    start_ = false;
    unompressedQueue_.Stop();
    compressedVector_.Stop();
    compressedContainerChanged_.notify_all();
}

DWORD CompressManager::Compress()
{
    try {
        auto exit = wil::scope_exit([this]
            {
                ++completeThreads_;
            });

        auto& que = unompressedQueue_;

        for (auto item = que.Pop(); item.second.get() != nullptr; item = que.Pop()) {
            auto compressBlock = CompressBlock(item.first, std::move(item.second));
            if (!compressBlock) {
                succeeded_ = false;
                compressedContainerChanged_.notify_all();
                return Return(35);
            }

            // wait for compressedVector_ is available
            compressedVector_.WaitInRange(item.first, compressed_vector_t::State::START);
            if (compressedVector_.state() != compressed_vector_t::State::START) {
                succeeded_ = false;
                compressedContainerChanged_.notify_all();
                return Return(36);
            }

            compressedVector_[item.first] = compressBlock;
            compressedVector_.IncrementOneElement();
            compressedVector_.CheckNotifyFull([this]
                {
                    compressedContainerChanged_.notify_all();
                });
        }

        return Return(0);
    } catch (uncompressed_queue_t::Failure&) {
        succeeded_ = false;
        compressedContainerChanged_.notify_all();
        return Return(30);
    } catch (...) {
        succeeded_ = false;
        compressedContainerChanged_.notify_all();
        return Return(31);
    }
}

CompressManager::shared_buffer_t CompressManager::CompressBlock(
    _In_ block_id          blockNo,
    _In_ shared_buffer_t&& uncompressBlock)
{
    auto compressBlock = std::make_shared<buffer_t>(uncompressBlock->Size() + 4 * 1024);

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
        uncompressBlock->Data(), static_cast<ULONG>(uncompressBlock->Size()),
        compressBlock->Data(), static_cast<ULONG>(compressBlock->Size()),
        4 * 1024, &compressSize, workSpace.get());

    DWORD lastError;
    if (ret == 0xC0000023) {
        lastError = ERROR_INSUFFICIENT_BUFFER;
    } else {
        lastError = ::GetLastError();
    }

    if (lastError == ERROR_INSUFFICIENT_BUFFER) {
        compressBlock->Resize(compressSize);
        ret = (*compressDll_.rtlCompressBuffer)(
            algo_ | COMPRESSION_ENGINE_MAXIMUM,
            uncompressBlock->Data(), static_cast<ULONG>(uncompressBlock->Size()),
            compressBlock->Data(), static_cast<ULONG>(compressBlock->Size()),
            4 * 1024, &compressSize, workSpace.get());
    }

    if (ret == 0) {
        compressBlock->Resize(compressSize);
        return compressBlock;
    } else {
        return shared_buffer_t(); // empty buffer
    }
}

void CompressManager::WaitCompress()
{
    if (!succeeded_) { return; }

    if (completeThreads_.load() < COMPRESS_THREAD_COUNT) {
        std::unique_lock<mutex_t> lock(mutex_);
        for (; succeeded_ && start_ && !compressedVector_.IsFull();) {
            compressedContainerChanged_.wait(lock);
        }
    }
}
