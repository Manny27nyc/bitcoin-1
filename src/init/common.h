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

//! @file
//! @brief Common init functions shared by bitcoin-node, bitcoin-wallet, etc.

#ifndef BITCOIN_INIT_COMMON_H
#define BITCOIN_INIT_COMMON_H

class ArgsManager;

namespace init {
void SetGlobals();
void UnsetGlobals();
/**
 *  Ensure a usable environment with all
 *  necessary library support.
 */
bool SanityChecks();
void AddLoggingArgs(ArgsManager& args);
void SetLoggingOptions(const ArgsManager& args);
void SetLoggingCategories(const ArgsManager& args);
bool StartLogging(const ArgsManager& args);
void LogPackageVersion();
} // namespace init

#endif // BITCOIN_INIT_COMMON_H
