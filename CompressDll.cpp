#include "CompressDll.h"

bool CompressDll::Init()
{
    if (inited) { return true; }

    if (!base_t::Init(L"ntdll.dll")) { return false; }

    rtlGetCompressionWorkspaceSize = reinterpret_cast<PfnRtlGetCompressionWorkspaceSize>(
        GetProcAddress(dll_.get(), "RtlGetCompressionWorkSpaceSize"));
    if (!rtlGetCompressionWorkspaceSize) { return false; }

    rtlCompressBuffer = reinterpret_cast<PfnRtlCompressBuffer>(
        GetProcAddress(dll_.get(), "RtlCompressBuffer"));
    if (!rtlCompressBuffer) { return false; }

    rtlDecompressBufferEx = reinterpret_cast<PfnRtlDecompressBufferEx>(
        GetProcAddress(dll_.get(), "RtlDecompressBufferEx"));
    if (!rtlDecompressBufferEx) { return false; }

    inited = true;

    return true;
}

CompressDll::~CompressDll()
{
    rtlDecompressBufferEx          = nullptr;
    rtlCompressBuffer              = nullptr;
    rtlGetCompressionWorkspaceSize = nullptr;
    inited                         = false;
}