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
# Copyright (c) 2017-2020 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test wallet replace-by-fee capabilities in conjunction with the fallbackfee."""

from test_framework.blocktools import COINBASE_MATURITY
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_raises_rpc_error

class WalletRBFTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def run_test(self):
        self.nodes[0].generate(COINBASE_MATURITY + 1)

        # sending a transaction without fee estimations must be possible by default on regtest
        self.nodes[0].sendtoaddress(self.nodes[0].getnewaddress(), 1)

        # test sending a tx with disabled fallback fee (must fail)
        self.restart_node(0, extra_args=["-fallbackfee=0"])
        assert_raises_rpc_error(-6, "Fee estimation failed", lambda: self.nodes[0].sendtoaddress(self.nodes[0].getnewaddress(), 1))
        assert_raises_rpc_error(-4, "Fee estimation failed", lambda: self.nodes[0].fundrawtransaction(self.nodes[0].createrawtransaction([], {self.nodes[0].getnewaddress(): 1})))
        assert_raises_rpc_error(-6, "Fee estimation failed", lambda: self.nodes[0].sendmany("", {self.nodes[0].getnewaddress(): 1}))

if __name__ == '__main__':
    WalletRBFTest().main()
