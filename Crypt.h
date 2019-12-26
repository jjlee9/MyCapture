#pragma once

#include <Windows.h>
#include <wil/resource.h>
#include "Implement.h"

class Crypt
{
public:
    bool Init(
        _In_ LPCWSTR algorithm);

    const wil::unique_bcrypt_algorithm& Algorithm() const { return algo_; }

    DWORD GetHashLength() const { return hashLength_; }

protected:
    wil::unique_bcrypt_algorithm algo_; // bcrypt algorithm
    DWORD                        hashLength_ = 0;
};

class Hash
{
public:
    using buffer_t = Implement::buffer_t;

    explicit Hash(const Crypt& crypt) :
        algo_(crypt.Algorithm()), hashLength_(crypt.GetHashLength())
    {
    }

    bool Create();

    _Success_(return)
    bool GetHash(
        _In_ const BYTE* input,
        _In_ ULONG       inputLength,
        _Out_ BYTE*      output) const;

protected:
    const wil::unique_bcrypt_algorithm& algo_;
    DWORD                               hashLength_ = 0;
    buffer_t                            hashObject_;
    wil::unique_bcrypt_hash             hashHandle_;
};