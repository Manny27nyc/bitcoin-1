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

#include <banman.h>
#include <fs.h>
#include <netaddress.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>
#include <test/util/setup_common.h>
#include <util/system.h>

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace {
int64_t ConsumeBanTimeOffset(FuzzedDataProvider& fuzzed_data_provider) noexcept
{
    // Avoid signed integer overflow by capping to int32_t max:
    // banman.cpp:137:73: runtime error: signed integer overflow: 1591700817 + 9223372036854775807 cannot be represented in type 'long'
    return fuzzed_data_provider.ConsumeIntegralInRange<int64_t>(std::numeric_limits<int64_t>::min(), std::numeric_limits<int32_t>::max());
}
} // namespace

void initialize_banman()
{
    static const auto testing_setup = MakeNoLogFileContext<>();
}

FUZZ_TARGET_INIT(banman, initialize_banman)
{
    // The complexity is O(N^2), where N is the input size, because each call
    // might call DumpBanlist (or other methods that are at least linear
    // complexity of the input size).
    int limit_max_ops{300};
    FuzzedDataProvider fuzzed_data_provider{buffer.data(), buffer.size()};
    SetMockTime(ConsumeTime(fuzzed_data_provider));
    const fs::path banlist_file = gArgs.GetDataDirNet() / "fuzzed_banlist.dat";
    fs::remove(banlist_file);
    {
        BanMan ban_man{banlist_file, nullptr, ConsumeBanTimeOffset(fuzzed_data_provider)};
        while (--limit_max_ops >= 0 && fuzzed_data_provider.ConsumeBool()) {
            CallOneOf(
                fuzzed_data_provider,
                [&] {
                    ban_man.Ban(ConsumeNetAddr(fuzzed_data_provider),
                                ConsumeBanTimeOffset(fuzzed_data_provider), fuzzed_data_provider.ConsumeBool());
                },
                [&] {
                    ban_man.Ban(ConsumeSubNet(fuzzed_data_provider),
                                ConsumeBanTimeOffset(fuzzed_data_provider), fuzzed_data_provider.ConsumeBool());
                },
                [&] {
                    ban_man.ClearBanned();
                },
                [&] {
                    ban_man.IsBanned(ConsumeNetAddr(fuzzed_data_provider));
                },
                [&] {
                    ban_man.IsBanned(ConsumeSubNet(fuzzed_data_provider));
                },
                [&] {
                    ban_man.Unban(ConsumeNetAddr(fuzzed_data_provider));
                },
                [&] {
                    ban_man.Unban(ConsumeSubNet(fuzzed_data_provider));
                },
                [&] {
                    banmap_t banmap;
                    ban_man.GetBanned(banmap);
                },
                [&] {
                    ban_man.DumpBanlist();
                },
                [&] {
                    ban_man.Discourage(ConsumeNetAddr(fuzzed_data_provider));
                });
        }
    }
    fs::remove(banlist_file);
}
