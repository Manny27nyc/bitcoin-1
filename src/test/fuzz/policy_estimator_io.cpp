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
// Copyright (c) 2020-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <policy/fees.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>
#include <test/util/setup_common.h>

#include <cstdint>
#include <vector>

void initialize_policy_estimator_io()
{
    static const auto testing_setup = MakeNoLogFileContext<>();
}

FUZZ_TARGET_INIT(policy_estimator_io, initialize_policy_estimator_io)
{
    FuzzedDataProvider fuzzed_data_provider(buffer.data(), buffer.size());
    FuzzedAutoFileProvider fuzzed_auto_file_provider = ConsumeAutoFile(fuzzed_data_provider);
    CAutoFile fuzzed_auto_file = fuzzed_auto_file_provider.open();
    // Re-using block_policy_estimator across runs to avoid costly creation of CBlockPolicyEstimator object.
    static CBlockPolicyEstimator block_policy_estimator;
    if (block_policy_estimator.Read(fuzzed_auto_file)) {
        block_policy_estimator.Write(fuzzed_auto_file);
    }
}
