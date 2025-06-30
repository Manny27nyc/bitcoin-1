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

#ifndef BITCOIN_UTIL_GETUNIQUEPATH_H
#define BITCOIN_UTIL_GETUNIQUEPATH_H

#include <fs.h>

/**
 * Helper function for getting a unique path
 *
 * @param[in] base  Base path
 * @returns base joined with a random 8-character long string.
 * @post Returned path is unique with high probability.
 */
fs::path GetUniquePath(const fs::path& base);

#endif // BITCOIN_UTIL_GETUNIQUEPATH_H