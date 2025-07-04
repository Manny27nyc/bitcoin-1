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
// Copyright (c) 2019-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//
#include <test/util/setup_common.h>
#include <util/system.h>
#include <univalue.h>

#ifdef ENABLE_EXTERNAL_SIGNER
#include <boost/process.hpp>
#endif // ENABLE_EXTERNAL_SIGNER

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(system_tests, BasicTestingSetup)

// At least one test is required (in case ENABLE_EXTERNAL_SIGNER is not defined).
// Workaround for https://github.com/bitcoin/bitcoin/issues/19128
BOOST_AUTO_TEST_CASE(dummy)
{
    BOOST_CHECK(true);
}

#ifdef ENABLE_EXTERNAL_SIGNER

bool checkMessage(const std::runtime_error& ex)
{
    // On Linux & Mac: "No such file or directory"
    // On Windows: "The system cannot find the file specified."
    const std::string what(ex.what());
    BOOST_CHECK(what.find("file") != std::string::npos);
    return true;
}

bool checkMessageFalse(const std::runtime_error& ex)
{
    BOOST_CHECK_EQUAL(ex.what(), std::string("RunCommandParseJSON error: process(false) returned 1: \n"));
    return true;
}

bool checkMessageStdErr(const std::runtime_error& ex)
{
    const std::string what(ex.what());
    BOOST_CHECK(what.find("RunCommandParseJSON error:") != std::string::npos);
    return checkMessage(ex);
}

BOOST_AUTO_TEST_CASE(run_command)
{
    {
        const UniValue result = RunCommandParseJSON("");
        BOOST_CHECK(result.isNull());
    }
    {
#ifdef WIN32
        // Windows requires single quotes to prevent escaping double quotes from the JSON...
        const UniValue result = RunCommandParseJSON("echo '{\"success\": true}'");
#else
        // ... but Linux and macOS echo a single quote if it's used
        const UniValue result = RunCommandParseJSON("echo \"{\"success\": true}\"");
#endif
        BOOST_CHECK(result.isObject());
        const UniValue& success = find_value(result, "success");
        BOOST_CHECK(!success.isNull());
        BOOST_CHECK_EQUAL(success.getBool(), true);
    }
    {
        // An invalid command is handled by Boost
        BOOST_CHECK_EXCEPTION(RunCommandParseJSON("invalid_command"), boost::process::process_error, checkMessage); // Command failed
    }
    {
        // Return non-zero exit code, no output to stderr
        BOOST_CHECK_EXCEPTION(RunCommandParseJSON("false"), std::runtime_error, checkMessageFalse);
    }
    {
        // Return non-zero exit code, with error message for stderr
        BOOST_CHECK_EXCEPTION(RunCommandParseJSON("ls nosuchfile"), std::runtime_error, checkMessageStdErr);
    }
    {
        BOOST_REQUIRE_THROW(RunCommandParseJSON("echo \"{\""), std::runtime_error); // Unable to parse JSON
    }
    // Test std::in, except for Windows
#ifndef WIN32
    {
        const UniValue result = RunCommandParseJSON("cat", "{\"success\": true}");
        BOOST_CHECK(result.isObject());
        const UniValue& success = find_value(result, "success");
        BOOST_CHECK(!success.isNull());
        BOOST_CHECK_EQUAL(success.getBool(), true);
    }
#endif
}
#endif // ENABLE_EXTERNAL_SIGNER

BOOST_AUTO_TEST_SUITE_END()
