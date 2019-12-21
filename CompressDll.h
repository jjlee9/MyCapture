#pragma once

#include "LoadDll.h"

struct CompressDll : public LoadDll
{
private:
    using base_t = LoadDll;

public:
    using PfnRtlCompressBuffer = NTSTATUS (WINAPI*)(
        _In_  USHORT CompressionFormatAndEngine,
        _In_  PUCHAR UncompressedBuffer,
        _In_  ULONG  UncompressedBufferSize,
        _Out_ PUCHAR CompressedBuffer,
        _In_  ULONG  CompressedBufferSize,
        _In_  ULONG  UncompressedChunkSize,
        _Out_ PULONG FinalCompressedSize,
        _In_  PVOID  WorkSpace);

    using PfnRtlGetCompressionWorkspaceSize = NTSTATUS (WINAPI*)(
        _In_  USHORT CompressionFormatAndEngine,
        _Out_ PULONG CompressBufferWorkSpaceSize,
        _Out_ PULONG CompressFragmentWorkSpaceSize);

    using PfnRtlDecompressBufferEx = NTSTATUS (WINAPI* )(
        _In_                                                                  USHORT CompressionFormat,
        _Out_writes_bytes_to_(UncompressedBufferSize, *FinalUncompressedSize) PUCHAR UncompressedBuffer,
        _In_                                                                  ULONG  UncompressedBufferSize,
        _In_reads_bytes_(CompressedBufferSize)                                PUCHAR CompressedBuffer,
        _In_                                                                  ULONG  CompressedBufferSize,
        _Out_                                                                 PULONG FinalUncompressedSize,
        _In_opt_                                                              PVOID  WorkSpace);

    ~CompressDll();

    bool Init();

    PfnRtlGetCompressionWorkspaceSize rtlGetCompressionWorkspaceSize = nullptr;
    PfnRtlCompressBuffer              rtlCompressBuffer              = nullptr;
    PfnRtlDecompressBufferEx          rtlDecompressBufferEx          = nullptr;
    bool                              inited                         = false;
};
