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
// Copyright (c) 2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/util/wallet.h>

#include <key_io.h>
#include <outputtype.h>
#include <script/standard.h>
#ifdef ENABLE_WALLET
#include <wallet/wallet.h>
#endif

const std::string ADDRESS_BCRT1_UNSPENDABLE = "bcrt1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq3xueyj";

#ifdef ENABLE_WALLET
std::string getnewaddress(CWallet& w)
{
    constexpr auto output_type = OutputType::BECH32;
    CTxDestination dest;
    std::string error;
    if (!w.GetNewDestination(output_type, "", dest, error)) assert(false);

    return EncodeDestination(dest);
}

void importaddress(CWallet& wallet, const std::string& address)
{
    auto spk_man = wallet.GetLegacyScriptPubKeyMan();
    LOCK2(wallet.cs_wallet, spk_man->cs_KeyStore);
    const auto dest = DecodeDestination(address);
    assert(IsValidDestination(dest));
    const auto script = GetScriptForDestination(dest);
    wallet.MarkDirty();
    assert(!spk_man->HaveWatchOnly(script));
    if (!spk_man->AddWatchOnly(script, 0 /* nCreateTime */)) assert(false);
    wallet.SetAddressBook(dest, /* label */ "", "receive");
}
#endif // ENABLE_WALLET
