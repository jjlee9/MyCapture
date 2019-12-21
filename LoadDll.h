#pragma once

#include <Windows.h>
#include <wil/resource.h>

class LoadDll
{
public:
    bool Init(
        _In_ const wchar_t* dllPath);

protected:
    wil::unique_hmodule dll_;
};
