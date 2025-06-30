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
// Copyright (c) 2021-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <i2p.h>
#include <netaddress.h>
#include <test/util/logging.h>
#include <test/util/net.h>
#include <test/util/setup_common.h>
#include <threadinterrupt.h>
#include <util/system.h>

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>

BOOST_FIXTURE_TEST_SUITE(i2p_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(unlimited_recv)
{
    auto CreateSockOrig = CreateSock;

    // Mock CreateSock() to create MockSock.
    CreateSock = [](const CService&) {
        return std::make_unique<StaticContentsSock>(std::string(i2p::sam::MAX_MSG_SIZE + 1, 'a'));
    };

    CThreadInterrupt interrupt;
    i2p::sam::Session session(gArgs.GetDataDirNet() / "test_i2p_private_key", CService{}, &interrupt);

    {
        ASSERT_DEBUG_LOG("Creating SAM session");
        ASSERT_DEBUG_LOG("too many bytes without a terminator");

        i2p::Connection conn;
        bool proxy_error;
        BOOST_REQUIRE(!session.Connect(CService{}, conn, proxy_error));
    }

    CreateSock = CreateSockOrig;
}

BOOST_AUTO_TEST_SUITE_END()
