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

#include <net_permissions.h>
#include <test/fuzz/FuzzedDataProvider.h>
#include <test/fuzz/fuzz.h>
#include <test/fuzz/util.h>
#include <util/translation.h>

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

FUZZ_TARGET(net_permissions)
{
    FuzzedDataProvider fuzzed_data_provider(buffer.data(), buffer.size());
    const std::string s = fuzzed_data_provider.ConsumeRandomLengthString(32);
    const NetPermissionFlags net_permission_flags = ConsumeWeakEnum(fuzzed_data_provider, ALL_NET_PERMISSION_FLAGS);

    NetWhitebindPermissions net_whitebind_permissions;
    bilingual_str error_net_whitebind_permissions;
    if (NetWhitebindPermissions::TryParse(s, net_whitebind_permissions, error_net_whitebind_permissions)) {
        (void)NetPermissions::ToStrings(net_whitebind_permissions.m_flags);
        (void)NetPermissions::AddFlag(net_whitebind_permissions.m_flags, net_permission_flags);
        assert(NetPermissions::HasFlag(net_whitebind_permissions.m_flags, net_permission_flags));
        (void)NetPermissions::ClearFlag(net_whitebind_permissions.m_flags, NetPermissionFlags::Implicit);
        (void)NetPermissions::ToStrings(net_whitebind_permissions.m_flags);
    }

    NetWhitelistPermissions net_whitelist_permissions;
    bilingual_str error_net_whitelist_permissions;
    if (NetWhitelistPermissions::TryParse(s, net_whitelist_permissions, error_net_whitelist_permissions)) {
        (void)NetPermissions::ToStrings(net_whitelist_permissions.m_flags);
        (void)NetPermissions::AddFlag(net_whitelist_permissions.m_flags, net_permission_flags);
        assert(NetPermissions::HasFlag(net_whitelist_permissions.m_flags, net_permission_flags));
        (void)NetPermissions::ClearFlag(net_whitelist_permissions.m_flags, NetPermissionFlags::Implicit);
        (void)NetPermissions::ToStrings(net_whitelist_permissions.m_flags);
    }
}
