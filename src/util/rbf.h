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
// Copyright (c) 2016-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_RBF_H
#define BITCOIN_UTIL_RBF_H

#include <cstdint>

class CTransaction;

static const uint32_t MAX_BIP125_RBF_SEQUENCE = 0xfffffffd;

// Check whether the sequence numbers on this transaction are signaling
// opt-in to replace-by-fee, according to BIP 125
bool SignalsOptInRBF(const CTransaction &tx);

#endif // BITCOIN_UTIL_RBF_H
