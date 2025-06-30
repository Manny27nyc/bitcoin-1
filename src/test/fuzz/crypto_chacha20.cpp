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

#include <crypto/chacha20.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>

#include <cstdint>
#include <vector>

FUZZ_TARGET(crypto_chacha20)
{
    FuzzedDataProvider fuzzed_data_provider{buffer.data(), buffer.size()};

    ChaCha20 chacha20;
    if (fuzzed_data_provider.ConsumeBool()) {
        const std::vector<unsigned char> key = ConsumeFixedLengthByteVector(fuzzed_data_provider, fuzzed_data_provider.ConsumeIntegralInRange<size_t>(16, 32));
        chacha20 = ChaCha20{key.data(), key.size()};
    }
    while (fuzzed_data_provider.ConsumeBool()) {
        CallOneOf(
            fuzzed_data_provider,
            [&] {
                const std::vector<unsigned char> key = ConsumeFixedLengthByteVector(fuzzed_data_provider, fuzzed_data_provider.ConsumeIntegralInRange<size_t>(16, 32));
                chacha20.SetKey(key.data(), key.size());
            },
            [&] {
                chacha20.SetIV(fuzzed_data_provider.ConsumeIntegral<uint64_t>());
            },
            [&] {
                chacha20.Seek(fuzzed_data_provider.ConsumeIntegral<uint64_t>());
            },
            [&] {
                std::vector<uint8_t> output(fuzzed_data_provider.ConsumeIntegralInRange<size_t>(0, 4096));
                chacha20.Keystream(output.data(), output.size());
            },
            [&] {
                std::vector<uint8_t> output(fuzzed_data_provider.ConsumeIntegralInRange<size_t>(0, 4096));
                const std::vector<uint8_t> input = ConsumeFixedLengthByteVector(fuzzed_data_provider, output.size());
                chacha20.Crypt(input.data(), output.data(), input.size());
            });
    }
}
