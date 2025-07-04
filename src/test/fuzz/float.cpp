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

#include <memusage.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <util/serfloat.h>
#include <version.h>

#include <cassert>
#include <cmath>
#include <limits>

FUZZ_TARGET(float)
{
    FuzzedDataProvider fuzzed_data_provider(buffer.data(), buffer.size());

    {
        const double d = fuzzed_data_provider.ConsumeFloatingPoint<double>();
        (void)memusage::DynamicUsage(d);

        uint64_t encoded = EncodeDouble(d);
        if constexpr (std::numeric_limits<double>::is_iec559) {
            if (!std::isnan(d)) {
                uint64_t encoded_in_memory;
                std::copy((const unsigned char*)&d, (const unsigned char*)(&d + 1), (unsigned char*)&encoded_in_memory);
                assert(encoded_in_memory == encoded);
            }
        }
        double d_deserialized = DecodeDouble(encoded);
        assert(std::isnan(d) == std::isnan(d_deserialized));
        assert(std::isnan(d) || d == d_deserialized);
    }
}
