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
# Copyright (c) 2017-2018 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test external signer.

Verify that a bitcoind node can use an external signer command.
See also wallet_signer.py for tests that require wallet context.
"""
import os
import platform

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_raises_rpc_error,
)


class RPCSignerTest(BitcoinTestFramework):
    def mock_signer_path(self):
        path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'mocks', 'signer.py')
        if platform.system() == "Windows":
            return "py " + path
        else:
            return path

    def set_test_params(self):
        self.num_nodes = 4

        self.extra_args = [
            [],
            [f"-signer={self.mock_signer_path()}", '-keypool=10'],
            [f"-signer={self.mock_signer_path()}", '-keypool=10'],
            ["-signer=fake.py"],
        ]

    def skip_test_if_missing_module(self):
        self.skip_if_no_external_signer()

    def set_mock_result(self, node, res):
        with open(os.path.join(node.cwd, "mock_result"), "w", encoding="utf8") as f:
            f.write(res)

    def clear_mock_result(self, node):
        os.remove(os.path.join(node.cwd, "mock_result"))

    def run_test(self):
        self.log.debug(f"-signer={self.mock_signer_path()}")

        assert_raises_rpc_error(-1, 'Error: restart bitcoind with -signer=<cmd>',
            self.nodes[0].enumeratesigners
        )

        # Handle script missing:
        assert_raises_rpc_error(-1, 'execve failed: No such file or directory',
            self.nodes[3].enumeratesigners
        )

        # Handle error thrown by script
        self.set_mock_result(self.nodes[1], "2")
        assert_raises_rpc_error(-1, 'RunCommandParseJSON error',
            self.nodes[1].enumeratesigners
        )
        self.clear_mock_result(self.nodes[1])

        self.set_mock_result(self.nodes[1], '0 [{"type": "trezor", "model": "trezor_t", "error": "fingerprint not found"}]')
        assert_raises_rpc_error(-1, 'fingerprint not found',
            self.nodes[1].enumeratesigners
        )
        self.clear_mock_result(self.nodes[1])

        result = self.nodes[1].enumeratesigners()
        assert_equal(len(result['signers']), 2)
        assert_equal(result['signers'][0]["fingerprint"], "00000001")
        assert_equal(result['signers'][0]["name"], "trezor_t")

if __name__ == '__main__':
    RPCSignerTest().main()
