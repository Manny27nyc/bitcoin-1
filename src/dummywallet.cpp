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
// Copyright (c) 2018-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/system.h>
#include <walletinitinterface.h>

class CWallet;

namespace interfaces {
class Chain;
class Handler;
class Wallet;
}

class DummyWalletInit : public WalletInitInterface {
public:

    bool HasWalletSupport() const override {return false;}
    void AddWalletOptions(ArgsManager& argsman) const override;
    bool ParameterInteraction() const override {return true;}
    void Construct(NodeContext& node) const override {LogPrintf("No wallet support compiled in!\n");}
};

void DummyWalletInit::AddWalletOptions(ArgsManager& argsman) const
{
    argsman.AddHiddenArgs({
        "-addresstype",
        "-avoidpartialspends",
        "-changetype",
        "-disablewallet",
        "-discardfee=<amt>",
        "-fallbackfee=<amt>",
        "-keypool=<n>",
        "-maxapsfee=<n>",
        "-maxtxfee=<amt>",
        "-mintxfee=<amt>",
        "-paytxfee=<amt>",
        "-rescan",
        "-salvagewallet",
        "-signer=<cmd>",
        "-spendzeroconfchange",
        "-txconfirmtarget=<n>",
        "-wallet=<path>",
        "-walletbroadcast",
        "-walletdir=<dir>",
        "-walletnotify=<cmd>",
        "-walletrbf",
        "-dblogsize=<n>",
        "-flushwallet",
        "-privdb",
        "-walletrejectlongchains",
        "-unsafesqlitesync",
    });
}

const WalletInitInterface& g_wallet_init_interface = DummyWalletInit();

namespace interfaces {

std::unique_ptr<Wallet> MakeWallet(const std::shared_ptr<CWallet>& wallet)
{
    throw std::logic_error("Wallet function called in non-wallet build.");
}

} // namespace interfaces
