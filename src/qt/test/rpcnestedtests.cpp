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
// Copyright (c) 2016-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/test/rpcnestedtests.h>

#include <interfaces/node.h>
#include <rpc/server.h>
#include <qt/rpcconsole.h>
#include <test/util/setup_common.h>
#include <univalue.h>
#include <util/system.h>

#include <QTest>

#include <string>
#include <stdexcept>

static RPCHelpMan rpcNestedTest_rpc()
{
    return RPCHelpMan{
        "rpcNestedTest",
        "echo the passed string(s)",
        {
            {"arg1", RPCArg::Type::STR, RPCArg::Optional::OMITTED, ""},
            {"arg2", RPCArg::Type::STR, RPCArg::Optional::OMITTED, ""},
            {"arg3", RPCArg::Type::STR, RPCArg::Optional::OMITTED, ""},
        },
        RPCResult{RPCResult::Type::ANY, "", ""},
        RPCExamples{""},
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue {
            return request.params.write(0, 0);
        },
    };
}

static const CRPCCommand vRPCCommands[] = {
    {"rpcNestedTest", &rpcNestedTest_rpc},
};

void RPCNestedTests::rpcNestedTests()
{
    // do some test setup
    // could be moved to a more generic place when we add more tests on QT level
    for (const auto& c : vRPCCommands) {
        tableRPC.appendCommand(c.name, &c);
    }

    TestingSetup test;
    m_node.setContext(&test.m_node);

    if (RPCIsInWarmup(nullptr)) SetRPCWarmupFinished();

    std::string result;
    std::string result2;
    std::string filtered;
    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo()[chain]", &filtered); //simple result filtering with path
    QVERIFY(result=="main");
    QVERIFY(filtered == "getblockchaininfo()[chain]");

    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblock(getbestblockhash())"); //simple 2 level nesting
    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblock(getblock(getbestblockhash())[hash], true)");

    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblock( getblock( getblock(getbestblockhash())[hash] )[hash], true)"); //4 level nesting with whitespace, filtering path and boolean parameter

    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo");
    QVERIFY(result.substr(0,1) == "{");

    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo()");
    QVERIFY(result.substr(0,1) == "{");

    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo "); //whitespace at the end will be tolerated
    QVERIFY(result.substr(0,1) == "{");

    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo()[\"chain\"]"); //Quote path identifier are allowed, but look after a child containing the quotes in the key
    QVERIFY(result == "null");

    RPCConsole::RPCExecuteCommandLine(m_node, result, "createrawtransaction [] {} 0"); //parameter not in brackets are allowed
    RPCConsole::RPCExecuteCommandLine(m_node, result2, "createrawtransaction([],{},0)"); //parameter in brackets are allowed
    QVERIFY(result == result2);
    RPCConsole::RPCExecuteCommandLine(m_node, result2, "createrawtransaction( [],  {} , 0   )"); //whitespace between parameters is allowed
    QVERIFY(result == result2);

    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblock(getbestblockhash())[tx][0]", &filtered);
    QVERIFY(result == "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    QVERIFY(filtered == "getblock(getbestblockhash())[tx][0]");

    RPCConsole::RPCParseCommandLine(nullptr, result, "importprivkey", false, &filtered);
    QVERIFY(filtered == "importprivkey(…)");
    RPCConsole::RPCParseCommandLine(nullptr, result, "signmessagewithprivkey abc", false, &filtered);
    QVERIFY(filtered == "signmessagewithprivkey(…)");
    RPCConsole::RPCParseCommandLine(nullptr, result, "signmessagewithprivkey abc,def", false, &filtered);
    QVERIFY(filtered == "signmessagewithprivkey(…)");
    RPCConsole::RPCParseCommandLine(nullptr, result, "signrawtransactionwithkey(abc)", false, &filtered);
    QVERIFY(filtered == "signrawtransactionwithkey(…)");
    RPCConsole::RPCParseCommandLine(nullptr, result, "walletpassphrase(help())", false, &filtered);
    QVERIFY(filtered == "walletpassphrase(…)");
    RPCConsole::RPCParseCommandLine(nullptr, result, "walletpassphrasechange(help(walletpassphrasechange(abc)))", false, &filtered);
    QVERIFY(filtered == "walletpassphrasechange(…)");
    RPCConsole::RPCParseCommandLine(nullptr, result, "help(encryptwallet(abc, def))", false, &filtered);
    QVERIFY(filtered == "help(encryptwallet(…))");
    RPCConsole::RPCParseCommandLine(nullptr, result, "help(importprivkey())", false, &filtered);
    QVERIFY(filtered == "help(importprivkey(…))");
    RPCConsole::RPCParseCommandLine(nullptr, result, "help(importprivkey(help()))", false, &filtered);
    QVERIFY(filtered == "help(importprivkey(…))");
    RPCConsole::RPCParseCommandLine(nullptr, result, "help(importprivkey(abc), walletpassphrase(def))", false, &filtered);
    QVERIFY(filtered == "help(importprivkey(…), walletpassphrase(…))");

    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest");
    QVERIFY(result == "[]");
    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest ''");
    QVERIFY(result == "[\"\"]");
    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest \"\"");
    QVERIFY(result == "[\"\"]");
    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest '' abc");
    QVERIFY(result == "[\"\",\"abc\"]");
    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest abc '' abc");
    QVERIFY(result == "[\"abc\",\"\",\"abc\"]");
    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest abc  abc");
    QVERIFY(result == "[\"abc\",\"abc\"]");
    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest abc\t\tabc");
    QVERIFY(result == "[\"abc\",\"abc\"]");
    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest(abc )");
    QVERIFY(result == "[\"abc\"]");
    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest( abc )");
    QVERIFY(result == "[\"abc\"]");
    RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest(   abc   ,   cba )");
    QVERIFY(result == "[\"abc\",\"cba\"]");

    QVERIFY_EXCEPTION_THROWN(RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo() .\n"), std::runtime_error); //invalid syntax
    QVERIFY_EXCEPTION_THROWN(RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo() getblockchaininfo()"), std::runtime_error); //invalid syntax
    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo("); //tolerate non closing brackets if we have no arguments
    RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo()()()"); //tolerate non command brackets
    QVERIFY_EXCEPTION_THROWN(RPCConsole::RPCExecuteCommandLine(m_node, result, "getblockchaininfo(True)"), UniValue); //invalid argument
    QVERIFY_EXCEPTION_THROWN(RPCConsole::RPCExecuteCommandLine(m_node, result, "a(getblockchaininfo(True))"), UniValue); //method not found
    QVERIFY_EXCEPTION_THROWN(RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest abc,,abc"), std::runtime_error); //don't tolerate empty arguments when using ,
    QVERIFY_EXCEPTION_THROWN(RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest(abc,,abc)"), std::runtime_error); //don't tolerate empty arguments when using ,
    QVERIFY_EXCEPTION_THROWN(RPCConsole::RPCExecuteCommandLine(m_node, result, "rpcNestedTest(abc,,)"), std::runtime_error); //don't tolerate empty arguments when using ,
}
