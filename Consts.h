#pragma once

#include <stdlib.h>

class Consts
{
public:
    static constexpr auto& OPTION_DRIVE_PATH = L"/CaptureDrive:";
    static constexpr auto& OPTION_IMAGE_FILE = L"/ImageFile:";
    static constexpr auto& OPTION_COMPRESS_ALGO = L"/CompressAlgorithm:";

    static constexpr auto OPTION_DRIVE_PATH_LEN = _countof(OPTION_DRIVE_PATH) - 1;
    static constexpr auto OPTION_IMAGE_FILE_LEN = _countof(OPTION_IMAGE_FILE) - 1;
    static constexpr auto OPTION_COMPRESS_ALGO_LEN = _countof(OPTION_COMPRESS_ALGO) - 1;
};