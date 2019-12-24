#include <Windows.h>
#include <chrono>
#include <tchar.h>
#include "Consts.h"
#include "Job.h"
#include "sequential/Job.h"

int __cdecl wmain(
    int      argc,
    wchar_t* argv[])
{
#if defined(_DEBUG) //|| defined(NDEBUG)
    auto start = std::chrono::system_clock::now();
#endif

    wchar_t drivePath[MAX_PATH] = {};
    wchar_t imageFile[MAX_PATH] = {};
    wchar_t algoNum[MAX_PATH] = { '4' };

    for (int i = 1; i < argc; ++i) {
        if (0 == _wcsnicmp(argv[i], Consts::OPTION_DRIVE_PATH, Consts::OPTION_DRIVE_PATH_LEN)) {
            // get DrivePath option in command line
            wcscpy_s(drivePath, &argv[i][Consts::OPTION_DRIVE_PATH_LEN]);
        } else if (0 == _wcsnicmp(argv[i], Consts::OPTION_IMAGE_FILE, Consts::OPTION_IMAGE_FILE_LEN)) {
            // get ImageFile option in command line
            wcscpy_s(imageFile, &argv[i][Consts::OPTION_IMAGE_FILE_LEN]);
        } else if (0 == _wcsnicmp(argv[i], Consts::OPTION_COMPRESS_ALGO, Consts::OPTION_COMPRESS_ALGO_LEN)) {
            // get CompressAlgorithm option in command line
            wcscpy_s(algoNum, &argv[i][Consts::OPTION_COMPRESS_ALGO_LEN]);
        } else {
        }
    }

    if (L'\0' == drivePath[0] || L'\0' == imageFile[0]) {
        wprintf(LR"(Syntax: MyCapture.exe /CaptureDrive:<source_drive> /ImageFile:<path_to_image> )");
        wprintf(LR"(    ex: MyCapture.exe /CaptureDrive:\\.\PhysicalDrive0 /ImageFile:C:\test.myu )");
        return 0;
    }

    {
        Job job;

        bool ok = job.Capture(drivePath, imageFile, _wtoi(algoNum));
        if (!ok) {
            return 1;
        }
    }

#if defined(_DEBUG) //|| defined(NDEBUG)
    auto end1 = std::chrono::system_clock::now();
    std::chrono::duration<double> secs = end1 - start;
    wprintf(L"elapsed time : %f secs\n", secs.count());
#endif

    {
        wcscat_s(imageFile, L"1");
        sequential::Job job;

        bool ok = job.Capture(drivePath, imageFile, _wtoi(algoNum));
        if (!ok) {
            return 2;
        }
    }

#if defined(_DEBUG) //|| defined(NDEBUG)
    auto end2 = std::chrono::system_clock::now();
    secs = end2 - end1;
    wprintf(L"elapsed time : %f secs\n", secs.count());
#endif

    {
        wcscat_s(imageFile, L"2");
        sequential::Job job;

        bool ok = job.Capture(drivePath, imageFile, _wtoi(algoNum),
            sequential::Job::compress{}, sequential::Job::calculate_hash{});
        if (!ok) {
            return 2;
        }
    }

#if defined(_DEBUG) //|| defined(NDEBUG)
    auto end3 = std::chrono::system_clock::now();
    secs = end3 - end2;
    wprintf(L"elapsed time : %f secs\n", secs.count());
#endif
}