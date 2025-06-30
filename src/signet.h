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
// Copyright (c) 2019-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SIGNET_H
#define BITCOIN_SIGNET_H

#include <consensus/params.h>
#include <primitives/block.h>
#include <primitives/transaction.h>

#include <optional>

/**
 * Extract signature and check whether a block has a valid solution
 */
bool CheckSignetBlockSolution(const CBlock& block, const Consensus::Params& consensusParams);

/**
 * Generate the signet tx corresponding to the given block
 *
 * The signet tx commits to everything in the block except:
 * 1. It hashes a modified merkle root with the signet signature removed.
 * 2. It skips the nonce.
 */
class SignetTxs {
    template<class T1, class T2>
    SignetTxs(const T1& to_spend, const T2& to_sign) : m_to_spend{to_spend}, m_to_sign{to_sign} { }

public:
    static std::optional<SignetTxs> Create(const CBlock& block, const CScript& challenge);

    const CTransaction m_to_spend;
    const CTransaction m_to_sign;
};

#endif // BITCOIN_SIGNET_H
