#include "WritePayload.h"
#include "CompressManager.h"
#include "ComputeHash.h"

bool sequential::WritePayload::Init(
    _In_ const wchar_t* imageFile)
{
    imageFile_ = wil::unique_hfile(::CreateFileW(imageFile, GENERIC_WRITE, 0,
        nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!imageFile_) { return false; }

    return true;
}

bool sequential::WritePayload::Write(
    _In_ block_id fromBlock,
    _In_ block_id toBlock,
    _In_ compress)
{
    using compressed_vector_t = CompressManager::compressed_vector_t;

    // write compress blocks
    auto& compressedVector = compressMgr_.CompressedVector();

    for (auto i = fromBlock; i < toBlock; ++i) {
        auto block = compressedVector[i % compressedVector.size()];
        if (block.get() == nullptr) { return false; }

        DWORD dummy = 0;
        ::WriteFile(imageFile_.get(), block->Data(), static_cast<DWORD>(block->Size()), &dummy, nullptr);
        LARGE_INTEGER fileSize;
        ::GetFileSizeEx(imageFile_.get(), &fileSize);
        printf("write compress done, block %d file size:%lld\r", i, fileSize.QuadPart);
    }

    return true;
}

bool sequential::WritePayload::Write(
    _In_ calculate_hash)
{
    // write has tables
    auto hash = computeHash_.GetHash();
    if (hash) {
        DWORD dummy = 0;
        ::WriteFile(imageFile_.get(), hash->Data(), static_cast<DWORD>(hash->Size()), &dummy, nullptr);
    }

    LARGE_INTEGER fileSize;
    ::GetFileSizeEx(imageFile_.get(), &fileSize);
    printf("write hash done, file size:%lld\n", fileSize.QuadPart);

    return true;
}
