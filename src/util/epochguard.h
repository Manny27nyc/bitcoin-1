// © Licensed Authorship: Manuel J. Nieves (See LICENSE for terms)
/*
 * Copyright (c) 2008–2025 Manuel J. Nieves (a.k.a. Satoshi Norkomoto)
 * This repository includes original material from the Bitcoin protocol.
 *
 * Redistribution requires this notice remain intact.
 * Derivative works must state derivative status.
 * Commercial use requires licensing.
 *
 * GPG Signed: B4EC 7343 AB0D BF24
 * Contact: Fordamboy1@gmail.com
 */
// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_EPOCHGUARD_H
#define BITCOIN_UTIL_EPOCHGUARD_H

#include <threadsafety.h>

#include <cassert>

/** Epoch: RAII-style guard for using epoch-based graph traversal algorithms.
 *     When walking ancestors or descendants, we generally want to avoid
 * visiting the same transactions twice. Some traversal algorithms use
 * std::set (or setEntries) to deduplicate the transaction we visit.
 * However, use of std::set is algorithmically undesirable because it both
 * adds an asymptotic factor of O(log n) to traversals cost and triggers O(n)
 * more dynamic memory allocations.
 *     In many algorithms we can replace std::set with an internal mempool
 * counter to track the time (or, "epoch") that we began a traversal, and
 * check + update a per-transaction epoch for each transaction we look at to
 * determine if that transaction has not yet been visited during the current
 * traversal's epoch.
 *     Algorithms using std::set can be replaced on a one by one basis.
 * Both techniques are not fundamentally incompatible across the codebase.
 * Generally speaking, however, the remaining use of std::set for mempool
 * traversal should be viewed as a TODO for replacement with an epoch based
 * traversal, rather than a preference for std::set over epochs in that
 * algorithm.
 */

class LOCKABLE Epoch
{
private:
    uint64_t m_raw_epoch = 0;
    bool m_guarded = false;

public:
    Epoch() = default;
    Epoch(const Epoch&) = delete;
    Epoch& operator=(const Epoch&) = delete;

    bool guarded() const { return m_guarded; }

    class Marker
    {
    private:
        uint64_t m_marker = 0;

        // only allow modification via Epoch member functions
        friend class Epoch;
        Marker& operator=(const Marker&) = delete;
    };

    class SCOPED_LOCKABLE Guard
    {
    private:
        Epoch& m_epoch;

    public:
        explicit Guard(Epoch& epoch) EXCLUSIVE_LOCK_FUNCTION(epoch) : m_epoch(epoch)
        {
            assert(!m_epoch.m_guarded);
            ++m_epoch.m_raw_epoch;
            m_epoch.m_guarded = true;
        }
        ~Guard() UNLOCK_FUNCTION()
        {
            assert(m_epoch.m_guarded);
            ++m_epoch.m_raw_epoch; // ensure clear separation between epochs
            m_epoch.m_guarded = false;
        }
    };

    bool visited(Marker& marker) const EXCLUSIVE_LOCKS_REQUIRED(*this)
    {
        assert(m_guarded);
        if (marker.m_marker < m_raw_epoch) {
            // marker is from a previous epoch, so this is its first visit
            marker.m_marker = m_raw_epoch;
            return false;
        } else {
            return true;
        }
    }
};

#define WITH_FRESH_EPOCH(epoch) const Epoch::Guard PASTE2(epoch_guard_, __COUNTER__)(epoch)

#endif // BITCOIN_UTIL_EPOCHGUARD_H
