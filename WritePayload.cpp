#include "WritePayload.h"
#include "CompressManager.h"
#include "ComputeHash.h"

bool WritePayload::Init(
    _In_ const wchar_t* imageFile)
{
    imageFile_ = wil::unique_hfile(::CreateFileW(imageFile, GENERIC_WRITE, 0,
        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!imageFile_) { return false; }

    Start();

    return true;
}

void WritePayload::Start()
{
    start_ = true;
}

void WritePayload::Stop()
{
    start_ = false;
}

DWORD WritePayload::Write(
    _In_ block_id totalBlocks)
{
    using compressed_vector_t = CompressManager::compressed_vector_t;

    try {
        static_assert(WRITE_PAYLOAD_THREAD_COUNT == 1, "WRITE_PAYLOAD_THREAD_COUNT should be 1");

        // write compress blocks
        auto& compressedVector = compressMgr_.CompressedVector();

        for (block_id i = 0; i < totalBlocks;) {
            compressMgr_.WaitCompress();

            auto limit = i + compressedVector.CurrentElements();
            for (auto j = i; j < limit; ++i, ++j) {
                auto block = compressedVector[j];
                if (block.get() == nullptr) { return Return(45); }

                DWORD dummy = 0;
                ::WriteFile(imageFile_.get(), block->Data(), static_cast<DWORD>(block->Size()), &dummy, nullptr);
                auto empty = compressedVector.uniqueInvoke(
                    [&compressedVector, j]
                    {
                        compressedVector[j].reset();
                        compressedVector.DecrementOneElement(j);
                        auto empty = compressedVector.IsEmpty();
                        if (empty) {
                            compressedVector.NextModulusBlock();
                        }
                        return empty;
                    });
                if (empty) {
                    compressedVector.NotifyReader();
                }
            }
            LARGE_INTEGER fileSize;
            ::GetFileSizeEx(imageFile_.get(), &fileSize);
            // should use i - 1 here, but
            printf("write compress done, block %d file size:%lld\r", i, fileSize.QuadPart);
        }
        printf("\n");

        // write has tables
        auto hash = computeHash_.WaitHash();
        if (hash) {
            DWORD dummy = 0;
            ::WriteFile(imageFile_.get(), hash->Data(), static_cast<DWORD>(hash->Size()), &dummy, nullptr);
        }

        LARGE_INTEGER fileSize;
        ::GetFileSizeEx(imageFile_.get(), &fileSize);
        printf("write hash done, file size:%lld\n", fileSize.QuadPart);

        return Return(0);
    } catch (compressed_vector_t::OutOfRange&) {
        return Return(40);
    } catch (compressed_vector_t::Failure&) {
        return Return(41);
    } catch (...) {
        return Return(42);
    }
}
