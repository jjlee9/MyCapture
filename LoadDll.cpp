#include "LoadDll.h"

bool LoadDll::Init(
    _In_ const wchar_t* dllPath)
{
    dll_ = wil::unique_hmodule(::LoadLibraryEx(dllPath, nullptr, 0));

    return dll_.get() != nullptr;
}
