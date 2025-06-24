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
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TEST_FUZZ_FUZZ_H
#define BITCOIN_TEST_FUZZ_FUZZ_H

#include <span.h>

#include <cstdint>
#include <functional>
#include <string_view>

using FuzzBufferType = Span<const uint8_t>;

using TypeTestOneInput = std::function<void(FuzzBufferType)>;
using TypeInitialize = std::function<void()>;
using TypeHidden = bool;

void FuzzFrameworkRegisterTarget(std::string_view name, TypeTestOneInput target, TypeInitialize init, TypeHidden hidden);

inline void FuzzFrameworkEmptyInitFun() {}

#define FUZZ_TARGET(name) \
    FUZZ_TARGET_INIT(name, FuzzFrameworkEmptyInitFun)

#define FUZZ_TARGET_INIT(name, init_fun) \
    FUZZ_TARGET_INIT_HIDDEN(name, init_fun, false)

#define FUZZ_TARGET_INIT_HIDDEN(name, init_fun, hidden)                               \
    void name##_fuzz_target(FuzzBufferType);                                          \
    struct name##_Before_Main {                                                       \
        name##_Before_Main()                                                          \
        {                                                                             \
            FuzzFrameworkRegisterTarget(#name, name##_fuzz_target, init_fun, hidden); \
        }                                                                             \
    } const static g_##name##_before_main;                                            \
    void name##_fuzz_target(FuzzBufferType buffer)

#endif // BITCOIN_TEST_FUZZ_FUZZ_H
