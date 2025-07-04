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

#include <test/util/logging.h>

#include <logging.h>
#include <noui.h>
#include <tinyformat.h>

#include <stdexcept>

DebugLogHelper::DebugLogHelper(std::string message, MatchFn match)
    : m_message{std::move(message)}, m_match(std::move(match))
{
    m_print_connection = LogInstance().PushBackCallback(
        [this](const std::string& s) {
            if (m_found) return;
            m_found = s.find(m_message) != std::string::npos && m_match(&s);
        });
    noui_test_redirect();
}

void DebugLogHelper::check_found()
{
    noui_reconnect();
    LogInstance().DeleteCallback(m_print_connection);
    if (!m_found && m_match(nullptr)) {
        throw std::runtime_error(strprintf("'%s' not found in debug log\n", m_message));
    }
}
