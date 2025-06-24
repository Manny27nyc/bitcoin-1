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
// Copyright (c) 2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <key.h>
#include <secp256k1.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>

#include <cstdint>
#include <vector>

bool SigHasLowR(const secp256k1_ecdsa_signature* sig);
int ecdsa_signature_parse_der_lax(const secp256k1_context* ctx, secp256k1_ecdsa_signature* sig, const unsigned char* input, size_t inputlen);

FUZZ_TARGET(secp256k1_ecdsa_signature_parse_der_lax)
{
    FuzzedDataProvider fuzzed_data_provider{buffer.data(), buffer.size()};
    const std::vector<uint8_t> signature_bytes = ConsumeRandomLengthByteVector(fuzzed_data_provider);
    if (signature_bytes.data() == nullptr) {
        return;
    }
    secp256k1_context* secp256k1_context_verify = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
    secp256k1_ecdsa_signature sig_der_lax;
    const bool parsed_der_lax = ecdsa_signature_parse_der_lax(secp256k1_context_verify, &sig_der_lax, signature_bytes.data(), signature_bytes.size()) == 1;
    if (parsed_der_lax) {
        ECC_Start();
        (void)SigHasLowR(&sig_der_lax);
        ECC_Stop();
    }
    secp256k1_context_destroy(secp256k1_context_verify);
}
