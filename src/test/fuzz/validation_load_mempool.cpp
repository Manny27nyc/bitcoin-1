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
// Copyright (c) 2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparamsbase.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>
#include <test/util/setup_common.h>
#include <txmempool.h>
#include <util/time.h>
#include <validation.h>

#include <cstdint>
#include <vector>

void initialize_validation_load_mempool()
{
    static const auto testing_setup = MakeNoLogFileContext<const TestingSetup>();
}

FUZZ_TARGET_INIT(validation_load_mempool, initialize_validation_load_mempool)
{
    FuzzedDataProvider fuzzed_data_provider{buffer.data(), buffer.size()};
    SetMockTime(ConsumeTime(fuzzed_data_provider));
    FuzzedFileProvider fuzzed_file_provider = ConsumeFile(fuzzed_data_provider);

    CTxMemPool pool{};
    auto fuzzed_fopen = [&](const fs::path&, const char*) {
        return fuzzed_file_provider.open();
    };
    (void)LoadMempool(pool, ::ChainstateActive(), fuzzed_fopen);
    (void)DumpMempool(pool, fuzzed_fopen, true);
}
