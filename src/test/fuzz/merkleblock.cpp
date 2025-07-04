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

#include <merkleblock.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>
#include <uint256.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

FUZZ_TARGET(merkleblock)
{
    FuzzedDataProvider fuzzed_data_provider(buffer.data(), buffer.size());
    CPartialMerkleTree partial_merkle_tree;
    CallOneOf(
        fuzzed_data_provider,
        [&] {
            const std::optional<CPartialMerkleTree> opt_partial_merkle_tree = ConsumeDeserializable<CPartialMerkleTree>(fuzzed_data_provider);
            if (opt_partial_merkle_tree) {
                partial_merkle_tree = *opt_partial_merkle_tree;
            }
        },
        [&] {
            CMerkleBlock merkle_block;
            const std::optional<CBlock> opt_block = ConsumeDeserializable<CBlock>(fuzzed_data_provider);
            CBloomFilter bloom_filter;
            std::set<uint256> txids;
            if (opt_block && !opt_block->vtx.empty()) {
                if (fuzzed_data_provider.ConsumeBool()) {
                    merkle_block = CMerkleBlock{*opt_block, bloom_filter};
                } else if (fuzzed_data_provider.ConsumeBool()) {
                    while (fuzzed_data_provider.ConsumeBool()) {
                        txids.insert(ConsumeUInt256(fuzzed_data_provider));
                    }
                    merkle_block = CMerkleBlock{*opt_block, txids};
                }
            }
            partial_merkle_tree = merkle_block.txn;
        });
    (void)partial_merkle_tree.GetNumTransactions();
    std::vector<uint256> matches;
    std::vector<unsigned int> indices;
    (void)partial_merkle_tree.ExtractMatches(matches, indices);
}
