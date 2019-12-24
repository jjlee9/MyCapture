#pragma once

#include <wil/resource.h>
#include "Implement.h"

namespace sequential
{

class CompressManager;
class ComputeHash;

class WritePayload
{
public:
    using block_id = Implement::block_id;

    explicit WritePayload(
        _In_ CompressManager& compressMgr,
        _In_ ComputeHash& computeHash) :
        compressMgr_(compressMgr),
        computeHash_(computeHash)
    {
    }

    bool Init(
        _In_ const wchar_t* imageFile);

    struct compress {};

    bool Write(
        _In_ block_id fromBlock,
        _In_ block_id toBlock,
        _In_ compress);

    struct calculate_hash {};

    bool Write(
        _In_ calculate_hash);

private:
    CompressManager&  compressMgr_;
    ComputeHash&      computeHash_;
    wil::unique_hfile imageFile_;
};

}; // namespace sequential
