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
"""Test fee filters during and after IBD."""

from decimal import Decimal

from test_framework.messages import COIN
from test_framework.test_framework import BitcoinTestFramework

MAX_FEE_FILTER = Decimal(9170997) / COIN
NORMAL_FEE_FILTER = Decimal(100) / COIN


class P2PIBDTxRelayTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 2
        self.extra_args = [
            ["-minrelaytxfee={}".format(NORMAL_FEE_FILTER)],
            ["-minrelaytxfee={}".format(NORMAL_FEE_FILTER)],
        ]

    def run_test(self):
        self.log.info("Check that nodes set minfilter to MAX_MONEY while still in IBD")
        for node in self.nodes:
            assert node.getblockchaininfo()['initialblockdownload']
            self.wait_until(lambda: all(peer['minfeefilter'] == MAX_FEE_FILTER for peer in node.getpeerinfo()))

        # Come out of IBD by generating a block
        self.nodes[0].generate(1)
        self.sync_all()

        self.log.info("Check that nodes reset minfilter after coming out of IBD")
        for node in self.nodes:
            assert not node.getblockchaininfo()['initialblockdownload']
            self.wait_until(lambda: all(peer['minfeefilter'] == NORMAL_FEE_FILTER for peer in node.getpeerinfo()))


if __name__ == '__main__':
    P2PIBDTxRelayTest().main()
