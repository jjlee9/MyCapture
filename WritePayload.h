#pragma once

#include <wil/resource.h>
#include "WorkThreads.h"
#include "Implement.h"

class CompressManager;
class ComputeHash;

class WritePayload
{
public:
    using block_id = Implement::block_id;

    static constexpr auto WRITE_PAYLOAD_THREAD_COUNT = WorkThreads::WRITE_PAYLOAD_THREAD_COUNT;

    explicit WritePayload(
        _In_ CompressManager& compressMgr,
        _In_ ComputeHash& computeHash) :
        compressMgr_(compressMgr),
        computeHash_(computeHash)
    {
    }

    bool Init(
        _In_ const wchar_t* imageFile);

    void Start();

    void Stop();

    DWORD Write(
        _In_ block_id totalBlocks);

private:
    DWORD Return(DWORD ret) const
    {
        if (start_) {
            return ret;
        } else {
            // the functionality is stopped intentionally, don't report any error
            return 0;
        }
    }

    CompressManager&  compressMgr_;
    ComputeHash&      computeHash_;
    bool              start_ = false;
    wil::unique_hfile imageFile_;
};
