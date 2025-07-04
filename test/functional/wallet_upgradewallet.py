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
"""upgradewallet RPC functional test

Test upgradewallet RPC. Download node binaries:

test/get_previous_releases.py -b v0.19.1 v0.18.1 v0.17.2 v0.16.3 v0.15.2

Only v0.15.2 and v0.16.3 are required by this test. The others are used in feature_backwards_compatibility.py
"""

import os
import shutil
import struct

from io import BytesIO

from test_framework.blocktools import COINBASE_MATURITY
from test_framework.bdb import dump_bdb_kv
from test_framework.messages import deser_compact_size, deser_string
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_equal,
    assert_is_hex_string,
    sha256sum_file,
)


UPGRADED_KEYMETA_VERSION = 12

def deser_keymeta(f):
    ver, create_time = struct.unpack('<Iq', f.read(12))
    kp_str = deser_string(f)
    seed_id = f.read(20)
    fpr = f.read(4)
    path_len = 0
    path = []
    has_key_orig = False
    if ver == UPGRADED_KEYMETA_VERSION:
        path_len = deser_compact_size(f)
        for i in range(0, path_len):
            path.append(struct.unpack('<I', f.read(4))[0])
        has_key_orig = bool(f.read(1))
    return ver, create_time, kp_str, seed_id, fpr, path_len, path, has_key_orig

class UpgradeWalletTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 3
        self.extra_args = [
            ["-addresstype=bech32", "-keypool=2"], # current wallet version
            ["-usehd=1", "-keypool=2"],            # v0.16.3 wallet
            ["-usehd=0", "-keypool=2"]             # v0.15.2 wallet
        ]
        self.wallet_names = [self.default_wallet_name, None, None]

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()
        self.skip_if_no_bdb()
        self.skip_if_no_previous_releases()

    def setup_network(self):
        self.setup_nodes()

    def setup_nodes(self):
        self.add_nodes(self.num_nodes, extra_args=self.extra_args, versions=[
            None,
            160300,
            150200,
        ])
        self.start_nodes()
        self.import_deterministic_coinbase_privkeys()

    def dumb_sync_blocks(self):
        """
        Little helper to sync older wallets.
        Notice that v0.15.2's regtest is hardforked, so there is
        no sync for it.
        v0.15.2 is only being used to test for version upgrade
        and master hash key presence.
        v0.16.3 is being used to test for version upgrade and balances.
        Further info: https://github.com/bitcoin/bitcoin/pull/18774#discussion_r416967844
        """
        node_from = self.nodes[0]
        v16_3_node = self.nodes[1]
        to_height = node_from.getblockcount()
        height = self.nodes[1].getblockcount()
        for i in range(height, to_height+1):
            b = node_from.getblock(blockhash=node_from.getblockhash(i), verbose=0)
            v16_3_node.submitblock(b)
        assert_equal(v16_3_node.getblockcount(), to_height)

    def test_upgradewallet(self, wallet, previous_version, requested_version=None, expected_version=None):
        unchanged = expected_version == previous_version
        new_version = previous_version if unchanged else expected_version if expected_version else requested_version
        assert_equal(wallet.getwalletinfo()["walletversion"], previous_version)
        assert_equal(wallet.upgradewallet(requested_version),
            {
                "wallet_name": "",
                "previous_version": previous_version,
                "current_version": new_version,
                "result": "Already at latest version. Wallet version unchanged." if unchanged else "Wallet upgraded successfully from version {} to version {}.".format(previous_version, new_version),
            }
        )
        assert_equal(wallet.getwalletinfo()["walletversion"], new_version)

    def test_upgradewallet_error(self, wallet, previous_version, requested_version, msg):
        assert_equal(wallet.getwalletinfo()["walletversion"], previous_version)
        assert_equal(wallet.upgradewallet(requested_version),
            {
                "wallet_name": "",
                "previous_version": previous_version,
                "current_version": previous_version,
                "error": msg,
            }
        )
        assert_equal(wallet.getwalletinfo()["walletversion"], previous_version)

    def run_test(self):
        self.nodes[0].generatetoaddress(COINBASE_MATURITY + 1, self.nodes[0].getnewaddress())
        self.dumb_sync_blocks()
        # # Sanity check the test framework:
        res = self.nodes[0].getblockchaininfo()
        assert_equal(res['blocks'], COINBASE_MATURITY + 1)
        node_master = self.nodes[0]
        v16_3_node  = self.nodes[1]
        v15_2_node  = self.nodes[2]

        # Send coins to old wallets for later conversion checks.
        v16_3_wallet  = v16_3_node.get_wallet_rpc('wallet.dat')
        v16_3_address = v16_3_wallet.getnewaddress()
        node_master.generatetoaddress(COINBASE_MATURITY + 1, v16_3_address)
        self.dumb_sync_blocks()
        v16_3_balance = v16_3_wallet.getbalance()

        self.log.info("Test upgradewallet RPC...")
        # Prepare for copying of the older wallet
        node_master_wallet_dir = os.path.join(node_master.datadir, "regtest/wallets", self.default_wallet_name)
        node_master_wallet = os.path.join(node_master_wallet_dir, self.default_wallet_name, self.wallet_data_filename)
        v16_3_wallet       = os.path.join(v16_3_node.datadir, "regtest/wallets/wallet.dat")
        v15_2_wallet       = os.path.join(v15_2_node.datadir, "regtest/wallet.dat")
        split_hd_wallet    = os.path.join(v15_2_node.datadir, "regtest/splithd")
        self.stop_nodes()

        # Make split hd wallet
        self.start_node(2, ['-usehd=1', '-keypool=2', '-wallet=splithd'])
        self.stop_node(2)

        def copy_v16():
            node_master.get_wallet_rpc(self.default_wallet_name).unloadwallet()
            # Copy the 0.16.3 wallet to the last Bitcoin Core version and open it:
            shutil.rmtree(node_master_wallet_dir)
            os.mkdir(node_master_wallet_dir)
            shutil.copy(
                v16_3_wallet,
                node_master_wallet_dir
            )
            node_master.loadwallet(self.default_wallet_name)

        def copy_non_hd():
            node_master.get_wallet_rpc(self.default_wallet_name).unloadwallet()
            # Copy the 0.15.2 non hd wallet to the last Bitcoin Core version and open it:
            shutil.rmtree(node_master_wallet_dir)
            os.mkdir(node_master_wallet_dir)
            shutil.copy(
                v15_2_wallet,
                node_master_wallet_dir
            )
            node_master.loadwallet(self.default_wallet_name)

        def copy_split_hd():
            node_master.get_wallet_rpc(self.default_wallet_name).unloadwallet()
            # Copy the 0.15.2 split hd wallet to the last Bitcoin Core version and open it:
            shutil.rmtree(node_master_wallet_dir)
            os.mkdir(node_master_wallet_dir)
            shutil.copy(
                split_hd_wallet,
                os.path.join(node_master_wallet_dir, 'wallet.dat')
            )
            node_master.loadwallet(self.default_wallet_name)

        self.restart_node(0)
        copy_v16()
        wallet = node_master.get_wallet_rpc(self.default_wallet_name)
        self.log.info("Test upgradewallet without a version argument")
        self.test_upgradewallet(wallet, previous_version=159900, expected_version=169900)
        # wallet should still contain the same balance
        assert_equal(wallet.getbalance(), v16_3_balance)

        copy_non_hd()
        wallet = node_master.get_wallet_rpc(self.default_wallet_name)
        # should have no master key hash before conversion
        assert_equal('hdseedid' in wallet.getwalletinfo(), False)
        self.log.info("Test upgradewallet with explicit version number")
        self.test_upgradewallet(wallet, previous_version=60000, requested_version=169900)
        # after conversion master key hash should be present
        assert_is_hex_string(wallet.getwalletinfo()['hdseedid'])

        self.log.info("Intermediary versions don't effect anything")
        copy_non_hd()
        # Wallet starts with 60000
        assert_equal(60000, wallet.getwalletinfo()['walletversion'])
        wallet.unloadwallet()
        before_checksum = sha256sum_file(node_master_wallet)
        node_master.loadwallet('')
        # Test an "upgrade" from 60000 to 129999 has no effect, as the next version is 130000
        self.test_upgradewallet(wallet, previous_version=60000, requested_version=129999, expected_version=60000)
        wallet.unloadwallet()
        assert_equal(before_checksum, sha256sum_file(node_master_wallet))
        node_master.loadwallet('')

        self.log.info('Wallets cannot be downgraded')
        copy_non_hd()
        self.test_upgradewallet_error(wallet, previous_version=60000, requested_version=40000,
            msg="Cannot downgrade wallet from version 60000 to version 40000. Wallet version unchanged.")
        wallet.unloadwallet()
        assert_equal(before_checksum, sha256sum_file(node_master_wallet))
        node_master.loadwallet('')

        self.log.info('Can upgrade to HD')
        # Inspect the old wallet and make sure there is no hdchain
        orig_kvs = dump_bdb_kv(node_master_wallet)
        assert b'\x07hdchain' not in orig_kvs
        # Upgrade to HD, no split
        self.test_upgradewallet(wallet, previous_version=60000, requested_version=130000)
        # Check that there is now a hd chain and it is version 1, no internal chain counter
        new_kvs = dump_bdb_kv(node_master_wallet)
        assert b'\x07hdchain' in new_kvs
        hd_chain = new_kvs[b'\x07hdchain']
        assert_equal(28, len(hd_chain))
        hd_chain_version, external_counter, seed_id = struct.unpack('<iI20s', hd_chain)
        assert_equal(1, hd_chain_version)
        seed_id = bytearray(seed_id)
        seed_id.reverse()
        old_kvs = new_kvs
        # First 2 keys should still be non-HD
        for i in range(0, 2):
            info = wallet.getaddressinfo(wallet.getnewaddress())
            assert 'hdkeypath' not in info
            assert 'hdseedid' not in info
        # Next key should be HD
        info = wallet.getaddressinfo(wallet.getnewaddress())
        assert_equal(seed_id.hex(), info['hdseedid'])
        assert_equal('m/0\'/0\'/0\'', info['hdkeypath'])
        prev_seed_id = info['hdseedid']
        # Change key should be the same keypool
        info = wallet.getaddressinfo(wallet.getrawchangeaddress())
        assert_equal(prev_seed_id, info['hdseedid'])
        assert_equal('m/0\'/0\'/1\'', info['hdkeypath'])

        self.log.info('Cannot upgrade to HD Split, needs Pre Split Keypool')
        for version in [139900, 159900, 169899]:
            self.test_upgradewallet_error(wallet, previous_version=130000, requested_version=version,
                msg="Cannot upgrade a non HD split wallet from version {} to version {} without upgrading to "
                    "support pre-split keypool. Please use version 169900 or no version specified.".format(130000, version))

        self.log.info('Upgrade HD to HD chain split')
        self.test_upgradewallet(wallet, previous_version=130000, requested_version=169900)
        # Check that the hdchain updated correctly
        new_kvs = dump_bdb_kv(node_master_wallet)
        hd_chain = new_kvs[b'\x07hdchain']
        assert_equal(32, len(hd_chain))
        hd_chain_version, external_counter, seed_id, internal_counter = struct.unpack('<iI20sI', hd_chain)
        assert_equal(2, hd_chain_version)
        assert_equal(0, internal_counter)
        seed_id = bytearray(seed_id)
        seed_id.reverse()
        assert_equal(seed_id.hex(), prev_seed_id)
        # Next change address is the same keypool
        info = wallet.getaddressinfo(wallet.getrawchangeaddress())
        assert_equal(prev_seed_id, info['hdseedid'])
        assert_equal('m/0\'/0\'/2\'', info['hdkeypath'])
        # Next change address is the new keypool
        info = wallet.getaddressinfo(wallet.getrawchangeaddress())
        assert_equal(prev_seed_id, info['hdseedid'])
        assert_equal('m/0\'/1\'/0\'', info['hdkeypath'])
        # External addresses use the same keypool
        info = wallet.getaddressinfo(wallet.getnewaddress())
        assert_equal(prev_seed_id, info['hdseedid'])
        assert_equal('m/0\'/0\'/3\'', info['hdkeypath'])

        self.log.info('Upgrade non-HD to HD chain split')
        copy_non_hd()
        self.test_upgradewallet(wallet, previous_version=60000, requested_version=169900)
        # Check that the hdchain updated correctly
        new_kvs = dump_bdb_kv(node_master_wallet)
        hd_chain = new_kvs[b'\x07hdchain']
        assert_equal(32, len(hd_chain))
        hd_chain_version, external_counter, seed_id, internal_counter = struct.unpack('<iI20sI', hd_chain)
        assert_equal(2, hd_chain_version)
        assert_equal(2, internal_counter)
        # Drain the keypool by fetching one external key and one change key. Should still be the same keypool
        info = wallet.getaddressinfo(wallet.getnewaddress())
        assert 'hdseedid' not in info
        assert 'hdkeypath' not in info
        info = wallet.getaddressinfo(wallet.getrawchangeaddress())
        assert 'hdseedid' not in info
        assert 'hdkeypath' not in info
        # The next addresses are HD and should be on different HD chains
        info = wallet.getaddressinfo(wallet.getnewaddress())
        ext_id = info['hdseedid']
        assert_equal('m/0\'/0\'/0\'', info['hdkeypath'])
        info = wallet.getaddressinfo(wallet.getrawchangeaddress())
        assert_equal(ext_id, info['hdseedid'])
        assert_equal('m/0\'/1\'/0\'', info['hdkeypath'])

        self.log.info('KeyMetadata should upgrade when loading into master')
        copy_v16()
        old_kvs = dump_bdb_kv(v16_3_wallet)
        new_kvs = dump_bdb_kv(node_master_wallet)
        for k, old_v in old_kvs.items():
            if k.startswith(b'\x07keymeta'):
                new_ver, new_create_time, new_kp_str, new_seed_id, new_fpr, new_path_len, new_path, new_has_key_orig = deser_keymeta(BytesIO(new_kvs[k]))
                old_ver, old_create_time, old_kp_str, old_seed_id, old_fpr, old_path_len, old_path, old_has_key_orig = deser_keymeta(BytesIO(old_v))
                assert_equal(10, old_ver)
                if old_kp_str == b"": # imported things that don't have keymeta (i.e. imported coinbase privkeys) won't be upgraded
                    assert_equal(new_kvs[k], old_v)
                    continue
                assert_equal(12, new_ver)
                assert_equal(new_create_time, old_create_time)
                assert_equal(new_kp_str, old_kp_str)
                assert_equal(new_seed_id, old_seed_id)
                assert_equal(0, old_path_len)
                assert_equal(new_path_len, len(new_path))
                assert_equal([], old_path)
                assert_equal(False, old_has_key_orig)
                assert_equal(True, new_has_key_orig)

                # Check that the path is right
                built_path = []
                for s in new_kp_str.decode().split('/')[1:]:
                    h = 0
                    if s[-1] == '\'':
                        s = s[:-1]
                        h = 0x80000000
                    p = int(s) | h
                    built_path.append(p)
                assert_equal(new_path, built_path)

        self.log.info('Upgrading to NO_DEFAULT_KEY should not remove the defaultkey')
        copy_split_hd()
        # Check the wallet has a default key initially
        old_kvs = dump_bdb_kv(node_master_wallet)
        defaultkey = old_kvs[b'\x0adefaultkey']
        self.log.info("Upgrade the wallet. Should still have the same default key.")
        self.test_upgradewallet(wallet, previous_version=139900, requested_version=159900)
        new_kvs = dump_bdb_kv(node_master_wallet)
        up_defaultkey = new_kvs[b'\x0adefaultkey']
        assert_equal(defaultkey, up_defaultkey)
        # 0.16.3 doesn't have a default key
        v16_3_kvs = dump_bdb_kv(v16_3_wallet)
        assert b'\x0adefaultkey' not in v16_3_kvs


if __name__ == '__main__':
    UpgradeWalletTest().main()
