#pragma once

#include <Windows.h>
#include <wil/resource.h>
#include "DiskConsts.h"
#include "Implement.h"

namespace sequential
{

struct ThreadParam;

class Job
{
public:
    using block_id = Implement::block_id;

    static constexpr auto BLOCK_LENGTH = DiskConsts::BLOCK_LENGTH;
    static constexpr auto READ_BLOCK_COUNT = DiskConsts::READ_BLOCK_COUNT;

    bool Capture(
        _In_ const wchar_t* drivePath,
        _In_ const wchar_t* imageFile,
        _In_ int            algo);

    struct compress {};
    struct calculate_hash {};

    bool Capture(
        _In_ const wchar_t* drivePath,
        _In_ const wchar_t* imageFile,
        _In_ int            algo,
        _In_ compress,
        _In_ calculate_hash);

private:
    bool DispatchThread(
        _In_ const wchar_t* imageFile,
        _In_ int            algo);

    bool DispatchThread(
        _In_ const wchar_t* imageFile,
        _In_ int            algo,
        _In_ compress,
        _In_ calculate_hash);

    wil::unique_hfile diskDrive_;
};

}; // namespace sequential
