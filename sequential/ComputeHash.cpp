#include "ComputeHash.h"

bool sequential::ComputeHash::Init(
    _In_ block_id totalBlocks)
{
    auto ret = sha256_.Init(BCRYPT_SHA256_ALGORITHM);
    if (!ret) { return ret; }

    hashTable_ = std::make_shared<hash_t>(static_cast<LONGLONG>(sha256_.GetHashLength()) * totalBlocks);

    return ret;
}

bool sequential::ComputeHash::Compute(
    _In_ block_id fromBlock,
    _In_ block_id toBlock)
{
    Hash hash(sha256_);

    if (!hash.Create()) {
        return false;
    }

    auto& que = calculateHashQueue_;

    for (auto i = fromBlock; i < toBlock; ++i) {
        auto item = que.front();
        que.pop();

        bool ok = hash.GetHash(item.second->Data(), static_cast<ULONG>(item.second->Size()),
            hashTable_->Data() + (sha256_.GetHashLength() * item.first));
        if (!ok) {
            return false;
        }
    }

    return true;
}

sequential::ComputeHash::shared_hash_t sequential::ComputeHash::GetHash() const
{
    return hashTable_;
}
