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
// Copyright (c) 2020-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <i2p.h>
#include <netaddress.h>
#include <netbase.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>
#include <test/util/setup_common.h>
#include <threadinterrupt.h>
#include <util/system.h>

void initialize_i2p()
{
    static const auto testing_setup = MakeNoLogFileContext<>();
}

FUZZ_TARGET_INIT(i2p, initialize_i2p)
{
    FuzzedDataProvider fuzzed_data_provider{buffer.data(), buffer.size()};

    // Mock CreateSock() to create FuzzedSock.
    auto CreateSockOrig = CreateSock;
    CreateSock = [&fuzzed_data_provider](const CService&) {
        return std::make_unique<FuzzedSock>(fuzzed_data_provider);
    };

    const CService sam_proxy;
    CThreadInterrupt interrupt;

    i2p::sam::Session sess{gArgs.GetDataDirNet() / "fuzzed_i2p_private_key", sam_proxy, &interrupt};

    i2p::Connection conn;

    if (sess.Listen(conn)) {
        if (sess.Accept(conn)) {
            try {
                (void)conn.sock->RecvUntilTerminator('\n', 10ms, interrupt, i2p::sam::MAX_MSG_SIZE);
            } catch (const std::runtime_error&) {
            }
        }
    }

    const CService to;
    bool proxy_error;

    if (sess.Connect(to, conn, proxy_error)) {
        try {
            conn.sock->SendComplete("verack\n", 10ms, interrupt);
        } catch (const std::runtime_error&) {
        }
    }

    CreateSock = CreateSockOrig;
}
