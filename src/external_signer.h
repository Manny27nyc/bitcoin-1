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
// Copyright (c) 2018-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_EXTERNAL_SIGNER_H
#define BITCOIN_EXTERNAL_SIGNER_H

#include <univalue.h>
#include <util/system.h>

#include <string>
#include <vector>

#ifdef ENABLE_EXTERNAL_SIGNER

struct PartiallySignedTransaction;

//! Enables interaction with an external signing device or service, such as
//! a hardware wallet. See doc/external-signer.md
class ExternalSigner
{
private:
    //! The command which handles interaction with the external signer.
    std::string m_command;

public:
    //! @param[in] command      the command which handles interaction with the external signer
    //! @param[in] fingerprint  master key fingerprint of the signer
    //! @param[in] chain        "main", "test", "regtest" or "signet"
    //! @param[in] name         device name
    ExternalSigner(const std::string& command, const std::string& fingerprint, const std::string chain, const std::string name);

    //! Master key fingerprint of the signer
    std::string m_fingerprint;

    //! Bitcoin mainnet, testnet, etc
    std::string m_chain;

    //! Name of signer
    std::string m_name;

    const std::string NetworkArg() const;

    //! Obtain a list of signers. Calls `<command> enumerate`.
    //! @param[in]              command the command which handles interaction with the external signer
    //! @param[in,out] signers  vector to which new signers (with a unique master key fingerprint) are added
    //! @param chain            "main", "test", "regtest" or "signet"
    //! @returns success
    static bool Enumerate(const std::string& command, std::vector<ExternalSigner>& signers, const std::string chain);

    //! Display address on the device. Calls `<command> displayaddress --desc <descriptor>`.
    //! @param[in] descriptor Descriptor specifying which address to display.
    //!            Must include a public key or xpub, as well as key origin.
    UniValue DisplayAddress(const std::string& descriptor) const;

    //! Get receive and change Descriptor(s) from device for a given account.
    //! Calls `<command> getdescriptors --account <account>`
    //! @param[in] account  which BIP32 account to use (e.g. `m/44'/0'/account'`)
    //! @returns see doc/external-signer.md
    UniValue GetDescriptors(const int account);

    //! Sign PartiallySignedTransaction on the device.
    //! Calls `<command> signtransaction` and passes the PSBT via stdin.
    //! @param[in,out] psbt  PartiallySignedTransaction to be signed
    bool SignTransaction(PartiallySignedTransaction& psbt, std::string& error);
};

#endif // ENABLE_EXTERNAL_SIGNER

#endif // BITCOIN_EXTERNAL_SIGNER_H
