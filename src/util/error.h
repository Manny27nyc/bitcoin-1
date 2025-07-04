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
// Copyright (c) 2010-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_ERROR_H
#define BITCOIN_UTIL_ERROR_H

/**
 * util/error.h is a common place for definitions of simple error types and
 * string functions. Types and functions defined here should not require any
 * outside dependencies.
 *
 * Error types defined here can be used in different parts of the
 * codebase, to avoid the need to write boilerplate code catching and
 * translating errors passed across wallet/node/rpc/gui code boundaries.
 */

#include <string>

struct bilingual_str;

enum class TransactionError {
    OK, //!< No error
    MISSING_INPUTS,
    ALREADY_IN_CHAIN,
    P2P_DISABLED,
    MEMPOOL_REJECTED,
    MEMPOOL_ERROR,
    INVALID_PSBT,
    PSBT_MISMATCH,
    SIGHASH_MISMATCH,
    MAX_FEE_EXCEEDED,
    EXTERNAL_SIGNER_NOT_FOUND,
    EXTERNAL_SIGNER_FAILED,
};

bilingual_str TransactionErrorString(const TransactionError error);

bilingual_str ResolveErrMsg(const std::string& optname, const std::string& strBind);

bilingual_str AmountHighWarn(const std::string& optname);

bilingual_str AmountErrMsg(const std::string& optname, const std::string& strValue);

#endif // BITCOIN_UTIL_ERROR_H
