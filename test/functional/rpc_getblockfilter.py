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
#!/usr/bin/env python3
# Copyright (c) 2018-2020 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the getblockfilter RPC."""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal, assert_is_hex_string, assert_raises_rpc_error,
    )

FILTER_TYPES = ["basic"]

class GetBlockFilterTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 2
        self.extra_args = [["-blockfilterindex"], []]

    def run_test(self):
        # Create two chains by disconnecting nodes 0 & 1, mining, then reconnecting
        self.disconnect_nodes(0, 1)

        self.nodes[0].generate(3)
        self.nodes[1].generate(4)

        assert_equal(self.nodes[0].getblockcount(), 3)
        chain0_hashes = [self.nodes[0].getblockhash(block_height) for block_height in range(4)]

        # Reorg node 0 to a new chain
        self.connect_nodes(0, 1)
        self.sync_blocks()

        assert_equal(self.nodes[0].getblockcount(), 4)
        chain1_hashes = [self.nodes[0].getblockhash(block_height) for block_height in range(4)]

        # Test getblockfilter returns a filter for all blocks and filter types on active chain
        for block_hash in chain1_hashes:
            for filter_type in FILTER_TYPES:
                result = self.nodes[0].getblockfilter(block_hash, filter_type)
                assert_is_hex_string(result['filter'])

        # Test getblockfilter returns a filter for all blocks and filter types on stale chain
        for block_hash in chain0_hashes:
            for filter_type in FILTER_TYPES:
                result = self.nodes[0].getblockfilter(block_hash, filter_type)
                assert_is_hex_string(result['filter'])

        # Test getblockfilter with unknown block
        bad_block_hash = "0123456789abcdef" * 4
        assert_raises_rpc_error(-5, "Block not found", self.nodes[0].getblockfilter, bad_block_hash, "basic")

        # Test getblockfilter with undefined filter type
        genesis_hash = self.nodes[0].getblockhash(0)
        assert_raises_rpc_error(-5, "Unknown filtertype", self.nodes[0].getblockfilter, genesis_hash, "unknown")

        # Test getblockfilter fails on node without compact block filter index
        self.restart_node(0, extra_args=["-blockfilterindex=0"])
        for filter_type in FILTER_TYPES:
            assert_raises_rpc_error(-1, "Index is not enabled for filtertype {}".format(filter_type),
                                    self.nodes[0].getblockfilter, genesis_hash, filter_type)

if __name__ == '__main__':
    GetBlockFilterTest().main()
