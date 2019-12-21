#include "ComputeHash.h"
#include <wil/resource.h>
#include <algorithm>

bool ComputeHash::Init(
    _In_ block_id totalBlocks)
{
    auto ret = sha256_.Init(BCRYPT_SHA256_ALGORITHM);
    hashTable_ = std::make_shared<hash_t>(static_cast<LONGLONG>(sha256_.GetHashLength()) * totalBlocks);
    completeThreads_ = 0;
    hashDone_.resize(0);
    hashDone_.resize(totalBlocks);

    return ret;
}

void ComputeHash::Start()
{
    start_ = true;
    calculateHashQueue_.Start();
}

void ComputeHash::Stop()
{
    start_ = false;
    calculateHashQueue_.Stop();
    hashFinished_.notify_all();
}

DWORD ComputeHash::Compute()
{
    try {
        auto exit = wil::scope_exit([this]
            {
                ++completeThreads_;

                // All threads are completed (compute hash finished), please notify all waiting targets!
                if (completeThreads_.load() == CALCULATE_HASH_THREAD_COUNT) {
                    hashFinished_.notify_all();
                }
            });

        Hash hash(sha256_);

        if (!hash.Create()) {
            return Return(25);
        }

        auto& que = calculateHashQueue_;

        for (auto item = que.Pop(); item.second.get() != nullptr; item = que.Pop()) {
            bool ok = hash.GetHash(item.second->Data(), static_cast<ULONG>(item.second->Size()),
                hashTable_->Data() + (sha256_.GetHashLength() * item.first));
            if (!ok) {
                hashFinished_.notify_all();
                return Return(26);
            }

            std::lock_guard<mutex_t> lock(mutex_);
            hashDone_[item.first] = true;
        }

        return Return(0);
    } catch (calculate_hash_queue_t::Failure&) {
        return Return(20);
    } catch (...) {
        return Return(21);
    }
}

ComputeHash::shared_hash_t ComputeHash::WaitHash()
{
    if (completeThreads_.load() < CALCULATE_HASH_THREAD_COUNT) {
        std::unique_lock<mutex_t> lock(mutex_);
        hashFinished_.wait(lock);
    }

    return GetHash();
}

ComputeHash::shared_hash_t ComputeHash::GetHash() const
{
    std::lock_guard<mutex_t> lock(mutex_);

    auto allDone = std::all_of(hashDone_.begin(), hashDone_.end(), [](bool v)
        {
            return v;
        });

    if (allDone) {
        return hashTable_;
    } else {
        return shared_hash_t();
    }
}
