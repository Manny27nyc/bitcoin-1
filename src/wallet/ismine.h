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
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_ISMINE_H
#define BITCOIN_WALLET_ISMINE_H

#include <script/standard.h>

#include <stdint.h>
#include <bitset>

class CWallet;
class CScript;

/**
 * IsMine() return codes, which depend on ScriptPubKeyMan implementation.
 * Not every ScriptPubKeyMan covers all types, please refer to
 * https://github.com/bitcoin/bitcoin/blob/master/doc/release-notes/release-notes-0.21.0.md#ismine-semantics
 * for better understanding.
 *
 * For LegacyScriptPubKeyMan,
 * ISMINE_NO: the scriptPubKey is not in the wallet;
 * ISMINE_WATCH_ONLY: the scriptPubKey has been imported into the wallet;
 * ISMINE_SPENDABLE: the scriptPubKey corresponds to an address owned by the wallet user (can spend with the private key);
 * ISMINE_USED: the scriptPubKey corresponds to a used address owned by the wallet user;
 * ISMINE_ALL: all ISMINE flags except for USED;
 * ISMINE_ALL_USED: all ISMINE flags including USED;
 * ISMINE_ENUM_ELEMENTS: the number of isminetype enum elements.
 *
 * For DescriptorScriptPubKeyMan and future ScriptPubKeyMan,
 * ISMINE_NO: the scriptPubKey is not in the wallet;
 * ISMINE_SPENDABLE: the scriptPubKey matches a scriptPubKey in the wallet.
 * ISMINE_USED: the scriptPubKey corresponds to a used address owned by the wallet user.
 *
 */
enum isminetype : unsigned int
{
    ISMINE_NO         = 0,
    ISMINE_WATCH_ONLY = 1 << 0,
    ISMINE_SPENDABLE  = 1 << 1,
    ISMINE_USED       = 1 << 2,
    ISMINE_ALL        = ISMINE_WATCH_ONLY | ISMINE_SPENDABLE,
    ISMINE_ALL_USED   = ISMINE_ALL | ISMINE_USED,
    ISMINE_ENUM_ELEMENTS,
};
/** used for bitflags of isminetype */
typedef uint8_t isminefilter;

/**
 * Cachable amount subdivided into watchonly and spendable parts.
 */
struct CachableAmount
{
    // NO and ALL are never (supposed to be) cached
    std::bitset<ISMINE_ENUM_ELEMENTS> m_cached;
    CAmount m_value[ISMINE_ENUM_ELEMENTS];
    inline void Reset()
    {
        m_cached.reset();
    }
    void Set(isminefilter filter, CAmount value)
    {
        m_cached.set(filter);
        m_value[filter] = value;
    }
};

#endif // BITCOIN_WALLET_ISMINE_H
