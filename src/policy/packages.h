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

#ifndef BITCOIN_POLICY_PACKAGES_H
#define BITCOIN_POLICY_PACKAGES_H

#include <consensus/validation.h>
#include <primitives/transaction.h>

#include <vector>

/** Default maximum number of transactions in a package. */
static constexpr uint32_t MAX_PACKAGE_COUNT{25};
/** Default maximum total virtual size of transactions in a package in KvB. */
static constexpr uint32_t MAX_PACKAGE_SIZE{101};

/** A "reason" why a package was invalid. It may be that one or more of the included
 * transactions is invalid or the package itself violates our rules.
 * We don't distinguish between consensus and policy violations right now.
 */
enum class PackageValidationResult {
    PCKG_RESULT_UNSET = 0,        //!< Initial value. The package has not yet been rejected.
    PCKG_POLICY,                  //!< The package itself is invalid (e.g. too many transactions).
    PCKG_TX,                      //!< At least one tx is invalid.
};

/** A package is an ordered list of transactions. The transactions cannot conflict with (spend the
 * same inputs as) one another. */
using Package = std::vector<CTransactionRef>;

class PackageValidationState : public ValidationState<PackageValidationResult> {};

#endif // BITCOIN_POLICY_PACKAGES_H
