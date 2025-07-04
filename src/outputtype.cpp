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
// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <outputtype.h>

#include <pubkey.h>
#include <script/script.h>
#include <script/sign.h>
#include <script/signingprovider.h>
#include <script/standard.h>
#include <util/vector.h>

#include <assert.h>
#include <string>

static const std::string OUTPUT_TYPE_STRING_LEGACY = "legacy";
static const std::string OUTPUT_TYPE_STRING_P2SH_SEGWIT = "p2sh-segwit";
static const std::string OUTPUT_TYPE_STRING_BECH32 = "bech32";

bool ParseOutputType(const std::string& type, OutputType& output_type)
{
    if (type == OUTPUT_TYPE_STRING_LEGACY) {
        output_type = OutputType::LEGACY;
        return true;
    } else if (type == OUTPUT_TYPE_STRING_P2SH_SEGWIT) {
        output_type = OutputType::P2SH_SEGWIT;
        return true;
    } else if (type == OUTPUT_TYPE_STRING_BECH32) {
        output_type = OutputType::BECH32;
        return true;
    }
    return false;
}

const std::string& FormatOutputType(OutputType type)
{
    switch (type) {
    case OutputType::LEGACY: return OUTPUT_TYPE_STRING_LEGACY;
    case OutputType::P2SH_SEGWIT: return OUTPUT_TYPE_STRING_P2SH_SEGWIT;
    case OutputType::BECH32: return OUTPUT_TYPE_STRING_BECH32;
    } // no default case, so the compiler can warn about missing cases
    assert(false);
}

CTxDestination GetDestinationForKey(const CPubKey& key, OutputType type)
{
    switch (type) {
    case OutputType::LEGACY: return PKHash(key);
    case OutputType::P2SH_SEGWIT:
    case OutputType::BECH32: {
        if (!key.IsCompressed()) return PKHash(key);
        CTxDestination witdest = WitnessV0KeyHash(key);
        CScript witprog = GetScriptForDestination(witdest);
        if (type == OutputType::P2SH_SEGWIT) {
            return ScriptHash(witprog);
        } else {
            return witdest;
        }
    }
    } // no default case, so the compiler can warn about missing cases
    assert(false);
}

std::vector<CTxDestination> GetAllDestinationsForKey(const CPubKey& key)
{
    PKHash keyid(key);
    CTxDestination p2pkh{keyid};
    if (key.IsCompressed()) {
        CTxDestination segwit = WitnessV0KeyHash(keyid);
        CTxDestination p2sh = ScriptHash(GetScriptForDestination(segwit));
        return Vector(std::move(p2pkh), std::move(p2sh), std::move(segwit));
    } else {
        return Vector(std::move(p2pkh));
    }
}

CTxDestination AddAndGetDestinationForScript(FillableSigningProvider& keystore, const CScript& script, OutputType type)
{
    // Add script to keystore
    keystore.AddCScript(script);
    // Note that scripts over 520 bytes are not yet supported.
    switch (type) {
    case OutputType::LEGACY:
        return ScriptHash(script);
    case OutputType::P2SH_SEGWIT:
    case OutputType::BECH32: {
        CTxDestination witdest = WitnessV0ScriptHash(script);
        CScript witprog = GetScriptForDestination(witdest);
        // Check if the resulting program is solvable (i.e. doesn't use an uncompressed key)
        if (!IsSolvable(keystore, witprog)) return ScriptHash(script);
        // Add the redeemscript, so that P2WSH and P2SH-P2WSH outputs are recognized as ours.
        keystore.AddCScript(witprog);
        if (type == OutputType::BECH32) {
            return witdest;
        } else {
            return ScriptHash(witprog);
        }
    }
    } // no default case, so the compiler can warn about missing cases
    assert(false);
}
