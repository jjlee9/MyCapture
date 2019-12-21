#include "Crypt.h"
#include <bcrypt.h>

bool Crypt::Init(
    _In_ LPCWSTR algorithm)
{
    // get algoritm handle
    auto status = ::BCryptOpenAlgorithmProvider(&algo_, algorithm, nullptr,
        BCRYPT_HASH_REUSABLE_FLAG);
    if (!BCRYPT_SUCCESS(status)) {
        return false;
    }

    DWORD dummy = 0;
    // get hash length
    status = ::BCryptGetProperty(algo_.get(), BCRYPT_HASH_LENGTH,
        reinterpret_cast<PBYTE>(&hashLength_), sizeof(hashLength_), &dummy, 0);
    if (!BCRYPT_SUCCESS(status)) {
        return false;
    }

    return true;
}

bool Hash::Create()
{
    DWORD dummy = 0;
    // get hash object length
    DWORD hashObjectLength = 0;
    auto status = ::BCryptGetProperty(algo_.get(), BCRYPT_OBJECT_LENGTH,
        reinterpret_cast<PBYTE>(&hashObjectLength), sizeof(hashObjectLength), &dummy, 0);
    if (!BCRYPT_SUCCESS(status)) {
        return false;
    }

    //create a hash
    hashObject_.Resize(hashObjectLength);
    status = ::BCryptCreateHash(algo_.get(), &hashHandle_, hashObject_.Data(), hashObjectLength,
        nullptr, 0, BCRYPT_HASH_REUSABLE_FLAG);
    if (!BCRYPT_SUCCESS(status)) {
        return false;
    }

    return true;
}

_Success_(return)
bool Hash::GetHash(
    _In_ const BYTE* input,
    _In_ ULONG       inputLength,
    _Out_ BYTE*      output) const
{
    auto status = ::BCryptHashData(hashHandle_.get(), const_cast<BYTE*>(input), inputLength, 0);
    if (!BCRYPT_SUCCESS(status)) {
        return false;
    }

    status = ::BCryptFinishHash(hashHandle_.get(), output, hashLength_, 0);
    if (!BCRYPT_SUCCESS(status)) {
        return false;
    }

    return true;
}
