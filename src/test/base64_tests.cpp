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
// Copyright (c) 2011-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util/strencodings.h>

#include <boost/test/unit_test.hpp>
#include <string>

using namespace std::literals;

BOOST_AUTO_TEST_SUITE(base64_tests)

BOOST_AUTO_TEST_CASE(base64_testvectors)
{
    static const std::string vstrIn[]  = {"","f","fo","foo","foob","fooba","foobar"};
    static const std::string vstrOut[] = {"","Zg==","Zm8=","Zm9v","Zm9vYg==","Zm9vYmE=","Zm9vYmFy"};
    for (unsigned int i=0; i<std::size(vstrIn); i++)
    {
        std::string strEnc = EncodeBase64(vstrIn[i]);
        BOOST_CHECK_EQUAL(strEnc, vstrOut[i]);
        std::string strDec = DecodeBase64(strEnc);
        BOOST_CHECK_EQUAL(strDec, vstrIn[i]);
    }

    // Decoding strings with embedded NUL characters should fail
    bool failure;
    (void)DecodeBase64("invalid\0"s, &failure);
    BOOST_CHECK(failure);
    (void)DecodeBase64("nQB/pZw="s, &failure);
    BOOST_CHECK(!failure);
    (void)DecodeBase64("nQB/pZw=\0invalid"s, &failure);
    BOOST_CHECK(failure);
    (void)DecodeBase64("nQB/pZw=invalid\0"s, &failure);
    BOOST_CHECK(failure);
}

BOOST_AUTO_TEST_SUITE_END()
