#pragma once

#include <Windows.h>
#include <wil/resource.h>
#include <vector>
#include "Implement.h"

class ReadDiskDrive;
class CompressManager;
class ComputeHash;
class WritePayload;

struct ThreadParam
{
    using block_id = Implement::block_id;
    using unique_thread_handle = wil::unique_any_handle_null_only<decltype(&::CloseHandle), ::CloseHandle>;

    block_id                          totalBlocks;
    ReadDiskDrive&                    readDiskDrive;
    CompressManager&                  compressMgr;
    ComputeHash&                      computeHash;
    WritePayload&                     writePayload;
    std::vector<unique_thread_handle> threads;
};
