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
# Copyright (c) 2020 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test GETDATA processing behavior"""
from collections import defaultdict

from test_framework.messages import (
    CInv,
    msg_getdata,
)
from test_framework.p2p import P2PInterface
from test_framework.test_framework import BitcoinTestFramework


class P2PStoreBlock(P2PInterface):
    def __init__(self):
        super().__init__()
        self.blocks = defaultdict(int)

    def on_block(self, message):
        message.block.calc_sha256()
        self.blocks[message.block.sha256] += 1


class GetdataTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1

    def run_test(self):
        p2p_block_store = self.nodes[0].add_p2p_connection(P2PStoreBlock())

        self.log.info("test that an invalid GETDATA doesn't prevent processing of future messages")

        # Send invalid message and verify that node responds to later ping
        invalid_getdata = msg_getdata()
        invalid_getdata.inv.append(CInv(t=0, h=0))  # INV type 0 is invalid.
        p2p_block_store.send_and_ping(invalid_getdata)

        # Check getdata still works by fetching tip block
        best_block = int(self.nodes[0].getbestblockhash(), 16)
        good_getdata = msg_getdata()
        good_getdata.inv.append(CInv(t=2, h=best_block))
        p2p_block_store.send_and_ping(good_getdata)
        p2p_block_store.wait_until(lambda: p2p_block_store.blocks[best_block] == 1)


if __name__ == '__main__':
    GetdataTest().main()
