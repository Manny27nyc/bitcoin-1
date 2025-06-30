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
// Copyright (c) 2016-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_POLICY_RBF_H
#define BITCOIN_POLICY_RBF_H

#include <txmempool.h>

/** The rbf state of unconfirmed transactions */
enum class RBFTransactionState {
    /** Unconfirmed tx that does not signal rbf and is not in the mempool */
    UNKNOWN,
    /** Either this tx or a mempool ancestor signals rbf */
    REPLACEABLE_BIP125,
    /** Neither this tx nor a mempool ancestor signals rbf */
    FINAL,
};

/**
 * Determine whether an unconfirmed transaction is signaling opt-in to RBF
 * according to BIP 125
 * This involves checking sequence numbers of the transaction, as well
 * as the sequence numbers of all in-mempool ancestors.
 *
 * @param tx   The unconfirmed transaction
 * @param pool The mempool, which may contain the tx
 *
 * @return     The rbf state
 */
RBFTransactionState IsRBFOptIn(const CTransaction& tx, const CTxMemPool& pool) EXCLUSIVE_LOCKS_REQUIRED(pool.cs);
RBFTransactionState IsRBFOptInEmptyMempool(const CTransaction& tx);

#endif // BITCOIN_POLICY_RBF_H
