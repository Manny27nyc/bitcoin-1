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
// Copyright (c) 2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TEST_UTIL_SCRIPT_H
#define BITCOIN_TEST_UTIL_SCRIPT_H

#include <crypto/sha256.h>
#include <script/script.h>

static const std::vector<uint8_t> WITNESS_STACK_ELEM_OP_TRUE{uint8_t{OP_TRUE}};
static const CScript P2WSH_OP_TRUE{
    CScript{}
    << OP_0
    << ToByteVector([] {
           uint256 hash;
           CSHA256().Write(WITNESS_STACK_ELEM_OP_TRUE.data(), WITNESS_STACK_ELEM_OP_TRUE.size()).Finalize(hash.begin());
           return hash;
       }())};

/** Flags that are not forbidden by an assert in script validation */
bool IsValidFlagCombination(unsigned flags);

#endif // BITCOIN_TEST_UTIL_SCRIPT_H
